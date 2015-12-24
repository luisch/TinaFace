// Listen for when the watchface is opened
var current_pos;
var watch_thr;

// ウェブサービスのID
var appid_yahoo = 'dj0zaiZpPU9IcEpWQXA5aGthWSZzPWNvbnN1bWVyc2VjcmV0Jng9ZTc-';
var appid_openweathermap = '372eb9ad7062f442eb2cbc69aa0857a2';

// func
var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.onerror = function() {
      sendData2Pebble({
        'KEY_TEMPERATURE': 99,
        'KEY_CONDITIONS': "network error",
        'KEY_CITY': "---",
      });
  };
  xhr.ontimeout = function() {
      sendData2Pebble({
        'KEY_TEMPERATURE': 99,
        'KEY_CONDITIONS': "timeout error",
        'KEY_CITY': "---",
      });
  };
  xhr.timeout = 3000;
  xhr.open(type, url);
  xhr.send();
};

function fetchLocation(pos){
  xhrRequest('http://reverse.search.olp.yahooapis.jp/OpenLocalPlatform/V1/reverseGeoCoder?output=json&'+pos+'&appid='+appid_yahoo, 'GET',
     function(r2){
       console.log('get location info from yahoo api at '+pos);
       var json = JSON.parse(r2);
       var city = json.Feature[0].Property.AddressElement[1];
       sendData2Pebble({
         'KEY_CITY': city ? city.Name : json.Feature[0].Property.Country.Name,
       });
     });
}

function fetchWeather(url) {
  // Send request to OpenWeatherMap
  xhrRequest('http://api.openweathermap.org/data/2.5/weather?appid='+appid_openweathermap+'&'+url, 'GET', 
    function(responseText) {
      // responseText contains a JSON object with weather info
      var json = JSON.parse(responseText);
      var temperature = Math.round(json.main.temp - 273.15);
      var conditions = json.weather[0].main;
      console.log('Temperature is ' + temperature);
      console.log('Conditions are ' + conditions);
      
      // Assemble dictionary using our keys
      sendData2Pebble({
        'KEY_TEMPERATURE': temperature,
        'KEY_CONDITIONS': conditions,
      });
    }
  );
}

function locationSuccess(pos) {
  // We will request the weather here
  console.log('query Weather info to openweathermap.org');
  current_pos = pos.coords;
  localStorage.setItem("last-position",JSON.stringify(current_pos));
  
  var cur_pos = 'lat='+ pos.coords.latitude + '&lon=' + pos.coords.longitude;
  fetchLocation(cur_pos);
  fetchWeather(cur_pos);
  
  if(!watch_thr){
    console.log('start position watching');
    watch_thr = navigator.geolocation.watchPosition(
      locationSuccess(),
      locationError(),
      {enableHighAccuracy: true, timeout: 100000, maximumAge: 200000}
    );
  }
}
function locationError(err) {
  console.log('Error requesting location!');
  if( current_pos ){
    var cur_pos = 'lat='+ current_pos.latitude + '&lon=' + current_pos.longitude;
    fetchLocation(cur_pos);
    fetchWeather(cur_pos);
  }
  else {
    // We will request the weather here
    fetchWeather('q=Japan');
    localStorage.removeItem("last-position");
  }
}

function initWeather() {
 current_pos = JSON.parse( localStorage.getItem("last-position") );
 if( current_pos ){
    console.log('init from localStorage');
    var cur_pos = 'lat='+ current_pos.latitude + '&lon=' + current_pos.longitude;
    fetchLocation(cur_pos);
    fetchWeather(cur_pos);    
  }

  console.log('query Location');
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {enableHighAccuracy: true, timeout: 100000, maximumAge: 200000}
    //新幹線移動中は60秒、列車移動中は30秒、静止状態で5秒程度で位置確保実績。
    //幅を取って、新幹線の役1.5倍程度でキリの良い100秒をタイムアウトにする。
  );
  
  console.log('start weather timer');
  setInterval(function(){
    console.log('query Location');
    navigator.geolocation.getCurrentPosition(
      locationSuccess,
      locationError,
      {enableHighAccuracy: true, timeout: 100000, maximumAge: 200000}
      //新幹線移動中は60秒、列車移動中は30秒、静止状態で5秒程度で位置確保実績。
      //幅を取って、新幹線の役1.5倍程度でキリの良い100秒をタイムアウトにする。
    );
  },5*60*1000);
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
//Pebble.addEventListener('appmessage',
//  function(e) {
//    
//  }                     
//);

Pebble.addEventListener('showConfiguration', function(e) {
  // Show config page
  Pebble.openURL('http://miko.moe/pebble/config.html'); // dummy
});
