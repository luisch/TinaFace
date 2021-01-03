// Listen for when the watchface is opened
var current_pos;
var update_gps_timeout = 60*3.75*1000; // 初期で3分45秒 -> 倍々して_maxにたどり着く
var update_gps_timeout_max = 60*120*1000; // 最大で120分

// ウェブサービスのID
var appid_yahoo = '';
var appid_openweathermap = '';

// wether id からアイコン文字コードに変換
function getWeatherCode( id ) {
      // http://openweathermap.org/weather-conditions
      // どうやら /uf102 まで表示可能で、それ以降はダメ文字らしい。なんで？
      var conditions;
      if(id < 300) { conditions = '\ue006';} // Thunder
      else if(id < 510) { conditions = '\ue004';} // RAIN
      else if(id < 600) { conditions = '\ue005';} // RAIN
      else if(id < 700) { conditions = '\ue007';} // SNOW
      else if(id < 800) { conditions = '\ue009';} // MIST
      else if(id < 802) { conditions = "\ue000";} // CLEAR
      else if(id < 804) { conditions = "\ue002";} // SCATTERED CLOUD
      else if(id < 900) { conditions = '\ue003';} // CLOUD
      else { conditions = '\ue008'; } //EXTREME

  console.log('Condition code is ' + escape(conditions));
  return conditions;
}

// func
var xhrRequest = function (url, type, callback) {
  if( navigator.onLine ){
    var xhr = new XMLHttpRequest();
    xhr.onload = function () {
      callback(this.responseText);
    };
    xhr.onerror = function() {
        update_gps_timeout = update_gps_timeout_max; // GPSアップデート頻度を最大限に落とす
    };
    xhr.ontimeout = function() {
        update_gps_timeout = update_gps_timeout_max; // GPSアップデート頻度を最大限に落とす
    };
    xhr.timeout = 3000;
    xhr.open(type, url);
    xhr.send();
  }
  else {
    console.log('Network is offline');
    update_gps_timeout = 60*3.75*1000; // GPSアップデート頻度頻度をあげる
  }
};

function fetchLocation(pos){
  xhrRequest('http://reverse.search.olp.yahooapis.jp/OpenLocalPlatform/V1/reverseGeoCoder?output=json&'+pos+'&appid='+appid_yahoo, 'GET',
     function(r2){
       console.log('get location info from yahoo api at '+pos);
       var json = JSON.parse(r2);
       var city = ".";
       var current_loc = ".";
       try {
         city = json.Feature[0].Property.AddressElement[1].Name;
         current_loc = city.Name;
         
         if( current_loc != JSON.parse( localStorage.getItem("last-location") ) ){
           localStorage.setItem("last-location",JSON.stringify(current_loc));
           sendData2Pebble({
             'KEY_CITY': city,
           });
           update_gps_timeout = 60*5*1000; // GPSアップデート頻度を元に戻す
         }
         else{
           console.log('pebble has remained in the neighborhood');
           update_gps_timeout = Math.min(update_gps_timeout*2, update_gps_timeout_max); // GPSアップデート頻度を落とす
         }
       }
       catch(e) {
         current_loc = pos;
         if( json.ResultInfo.Count > 0 ){
           console.log('pebble is over seas.');
           city = json.Feature[0].Property.Country.Name;
         }
         else {
           console.log('pebble is public seas.');
           city = "公海";
         }
         localStorage.setItem("last-location",JSON.stringify(current_loc));
         sendData2Pebble({
           'KEY_CITY': city,
         });
         update_gps_timeout = 60*60*1000; // GPSアップデート頻度を海外仕様にする
         localStorage.removeItem("last-position");
       }
     });
}
function fetchWeather(url) {
  // Send request to OpenWeatherMap
  console.log(url);
  xhrRequest('http://api.openweathermap.org/data/2.5/weather?appid='+appid_openweathermap+'&'+url, 'GET', 
    function(responseText) {
      console.log('received from openweather map');
      // responseText contains a JSON object with weather info
      var json = JSON.parse(responseText);
      var temperature = (Math.round(10*(json.main.temp - 273.15))/10).toString();
      var conditions = getWeatherCode(json.weather[0].id);
      var wind_speed = json.wind.speed;
      console.log('Temperature is ' + temperature);
      console.log('Conditions are ' + json.weather[0].main +"("+ json.weather[0].id +")");
      console.log('Wind are ' + wind_speed);
      
      // Assemble dictionary using our keys
      sendData2Pebble({
        'KEY_TEMPERATURE': temperature,
        'KEY_CONDITIONS': conditions,
        'KEY_WINDSPEED': wind_speed.toString(),
      });
      xhrRequest('http://api.openweathermap.org/data/2.5/forecast?appid='+appid_openweathermap+'&'+url, 'GET', 
        function(responseText) {
          var json = JSON.parse(responseText);
          console.log('Forecast Conditions are ' + json.list[0].weather[0].main +"("+ json.list[0].weather[0].id +")");
          console.log('Forecast Conditions are ' + json.list[2].weather[0].main +"("+ json.list[1].weather[0].id +")");
          sendData2Pebble({
            'KEY_FORECAST': getWeatherCode(json.list[0].weather[0].id) + " " + getWeatherCode(json.list[2].weather[0].id),
          });
        }
      );
    }
  );
}

function locationSuccess(pos) {
  // We will request the weather here
  console.log('query Weather info to openweathermap.org');
  current_pos = pos;
  localStorage.setItem("last-position",JSON.stringify(pos));
  
  var cur_pos = 'lat='+ pos.coords.latitude + '&lon=' + pos.coords.longitude;
  fetchLocation(cur_pos);
  fetchWeather(cur_pos);
}
function locationError(err) {
  console.log('Error requesting location!');
  if( current_pos ){
    var cur_pos = 'lat='+ current_pos.coords.latitude + '&lon=' + current_pos.coords.longitude;
    fetchLocation(cur_pos);
    fetchWeather(cur_pos);
  }
  else {
    // We will request the weather here
    fetchWeather('q=Japan');
    localStorage.removeItem("last-position");
  }
}
function updateLocation()
{
  if( navigator.onLine ){
    console.log('update Location');
    navigator.geolocation.getCurrentPosition(
      locationSuccess,
      locationError,
      {enableHighAccuracy: true, timeout: 100000, maximumAge: 200000}
      //新幹線移動中は60秒、列車移動中は30秒、静止状態で5秒程度で位置確保実績。
      //幅を取って、新幹線の約1.5倍程度でキリの良い100秒をタイムアウトにする。
    );
    console.log('next update after ' + update_gps_timeout/1000 + ' [s]');
  }
  else {
    console.log('Network is offline');
    update_gps_timeout = 60*3.75*1000; // GPSアップデート頻度頻度をあげる
  }
  setTimeout(updateLocation, update_gps_timeout);
}

function initWeather() {
  // 起動した直後なので、表示が消えている
  current_pos = JSON.parse( localStorage.getItem("last-position") );
  console.dir(current_pos);
  if( current_pos && current_pos.coords && current_pos.coords.latitude && current_pos.coords.longitude ){
    console.log('init from localStorage');
    var cur_pos = 'lat='+ current_pos.coords.latitude + '&lon=' + current_pos.coords.longitude;
//  fetchLocation(cur_pos);
    fetchWeather(cur_pos);    
  }
  else {
    localStorage.removeItem("last-position");
  }
  updateLocation();
}

function sendData2Pebble(dictionary)
{
      // Send to Pebble
      Pebble.sendAppMessage(dictionary,
        function(e) {
          console.log('sent data to Pebble successfully!');
        },
        function(e) {
          console.log('Error sending data to Pebble');
        }
      );
}

Pebble.addEventListener('ready', 
  function(e) {
//    localStorage.removeItem('last-position');
    console.log('PebbleKit JS ready!');
    initWeather();
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('refresh',
  function(e){
    console.log('Received Refresh request');
    var last_loc = JSON.parse( localStorage.getItem("last-location"));
    var last_pos = JSON.parse( localStorage.getItem("last-position") );
    if(last_loc){
      sendData2Pebble({
        'KEY_CITY': last_loc,
      });
    }
    if( last_pos ){
      console.log('init from localStorage');
      var cur_pos = 'lat='+ last_pos.latitude + '&lon=' + last_pos.longitude;
      fetchWeather(cur_pos);
    }
    else {
      if( navigator.onLine ){
        console.log('update Location');
        navigator.geolocation.getCurrentPosition(
          locationSuccess,
          locationError,
          {enableHighAccuracy: true, timeout: 100000, maximumAge: 200000}
          //新幹線移動中は60秒、列車移動中は30秒、静止状態で5秒程度で位置確保実績。
          //幅を取って、新幹線の約1.5倍程度でキリの良い100秒をタイムアウトにする。
        );
      }
      else {
        console.log('Network is offline');
        update_gps_timeout = 60*3.75*1000; // GPSアップデート頻度頻度をあげる
      }
    }
  }
);

Pebble.addEventListener('showConfiguration', function(e) {
  // Show config page
  Pebble.openURL('http://miko.moe/pebble/config.html'); // dummy
});
