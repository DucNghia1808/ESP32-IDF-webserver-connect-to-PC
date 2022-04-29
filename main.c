#include <stdio.h>
#include <stdint.h> // thu vien xu ly ki tu
#include <stddef.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_timer.h" // import thư viện timer
#include "driver/adc.h" // thư viện adc
#include "esp_adc_cal.h"

#include "cJSON.h" // thu vien json
#include "driver/uart.h" // driver uart
#include "dht11.h"

////
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_tls.h"
/*#include "esp_netif.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/api.h"*/
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_http_client.h"
#include <inttypes.h>
#include <sys/param.h>
#include <esp_http_server.h>

static const int RX_BUF_SIZE = 1024; //uart
#define TXD2 17
#define RXD2 16

#define DEN 2
#define QUAT 4
#define DEN1 15
#define QUAT1 5


#define NUT1 19
#define NUT2 21
#define NUT3 22
#define NUT4 23

cJSON *str_json;
int nhietdo = 0; int doam = 0;
int TB1 = 0;int TB2 = 0;
int TB3 = 0;int TB4 = 0;
int C1 = 100;int C2 = 200;
char JSON1[100];

#define EXAMPLE_ESP_WIFI_SSID      "J19"
#define EXAMPLE_ESP_WIFI_PASS      "0948518286"
#define EXAMPLE_ESP_MAXIMUM_RETRY  5
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static int s_retry_num = 0;
static const char *TAG1 = "wifi station";
static EventGroupHandle_t s_wifi_event_group;

static const char *TAG = "http-Server-Webserver";
static httpd_handle_t server = NULL;

//const char *webpage = "<!DOCTYPE html> <html> <head> <meta charset=\"UTF-8\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> <title>nhacuanghia</title> </head> <style> body{ text-align: center; } #hide{ margin:10px; padding: 25px; color: white; display: none; } #login{ width: 300px; height: 280px; border: 3px solid grey; border-radius: 15px; text-align: center; margin: 0 auto; } #login h5{ color:#1b2020; font-family: sans-serif; } #login input{ width: 170px; height: 35px; margin-bottom: 10px; border-radius: 7px; border: 1px solid grey; padding-left: 20px; } #login button{ width:100px; height: 40px; margin-bottom: 10px; border-radius: 12px; background-color: rgb(40, 119, 129); color: white; } button:hover{ font-size: 14px; } .switch { position: relative; display: inline-block; width: 50px; height: 23px; } .switch input { opacity: 0; width: 0; height: 0; } .slider { position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; -webkit-transition: .4s; transition: .4s; } .slider:before { position: absolute; content: \"\"; height: 16px; width: 16px; left: 4px; bottom: 4px; background-color: white; -webkit-transition: .4s; transition: .4s; } input:checked + .slider { background-color: #2196F3; } input:focus + .slider { box-shadow: 0 0 1px #2196F3; } input:checked + .slider:before { -webkit-transform: translateX(26px); -ms-transform: translateX(26px); transform: translateX(26px); } /* Rounded sliders */ .slider.round { border-radius: 34px; } .slider.round:before { border-radius: 50%; } button{ width:80px; height: 30px; border-radius: 7px; } table{ text-align: center; margin: 0 auto; background: rgb(32, 138, 138); height: 75%; width: 500px; height:200px; } tr{ height: 50px; } </style> <body onload=\"LoadData()\"> <div id=\"login\"> <h2> MY HOME </h2> <h5>Tài khoản <input type=\" text\" placeholder=\"username\" id=\"Username\"> </h5> <h5>Mật khẩu <input type=\"password\" placeholder=\"password\" id=\"Password\"> </h5> <button id=\"myBtn\" onclick=\"check_input()\">Đăng Nhập</button> </div> <div id=\"hide\"> <table border=\"2\" > <tr style=\" height:100px;\"> <td colspan=\"4\" >ESP32-IDF WEBSERVER</td> </tr> <tr> <td>THIẾT BỊ</td> <td colspan=\"3\" >TRẠNG THÁI</td> </tr> <tr> <td>THIẾT BỊ 1</td> <td> <label class=\"switch\" id=\"switch1\" onclick=\"DK_DEN()\"> <input id=\"inputSwitch1\" type=\"checkbox\" > <span class=\"slider round\"></span> </label> </td> <td><label id=\"TB1\">OFF</label></td> <td rowspan=\"2\"> Nhiệt Độ <br> <label id=\"nhietdo\">0</label> <label > &#176;C</label> </td> </tr> <tr> <td>THIẾT BỊ 2</td> <td> <label class=\"switch\" id=\"switch2\" onclick=\"DK_QUAT()\"> <input id=\"inputSwitch2\" type=\"checkbox\" > <span class=\"slider round\"></span> </label> </td> <td><label id=\"TB2\">OFF</label></td> </tr> <tr> <td>THIẾT BỊ 3</td> <td> <label class=\"switch\" id=\"switch3\" onclick=\"DK_DEN1()\"> <input id=\"inputSwitch3\" type=\"checkbox\" > <span class=\"slider round\"></span> </label> </td> <td><label id=\"TB3\">OFF</label></td> <td rowspan=\"2\"> Độ Ẩm <br> <label id=\"doam\">0</label> <label > % </label> </td> </tr> <tr> <td>THIẾT BỊ 4</td> <td> <label class=\"switch\" id=\"switch4\" onclick=\"DK_QUAT1()\"> <input id=\"inputSwitch4\" type=\"checkbox\" > <span class=\"slider round\"></span> </label> </td> <td><label id=\"TB4\">OFF</label></td> </tr> <tr> <td><input id=\"cai1\"></td> <td> <button onclick=\"SendC1()\">Send</button> </label> </td> </tr> <tr> <td><input id=\"cai2\"></td> <td> <button onclick=\"SendC2()\">Send</button> </label> </td> </tr> <tr> <td colspan=\"3\"> <button id=\"reload\" onclick=\"lockpage()\">Thoát</button> </td> </tr> </table> </div> </body> <script> var dataON = !document.getElementById(\"inputSwitch1\").checked; var dataOFF = document.getElementById(\"inputSwitch1\").checked; function LoadData() { var xhttp = new XMLHttpRequest(); xhttp.onreadystatechange = function() { if (this.readyState == 4 && this.status == 200) { var DataVDK = xhttp.responseText; console.log(\"Dữ liệu VDK:\" + DataVDK); var DataJson = JSON.parse(DataVDK); document.getElementById(\"nhietdo\").innerHTML = DataJson.ND; document.getElementById(\"doam\").innerHTML = DataJson.DA; if(DataJson.TB1 == \"0\") { document.getElementById(\"TB1\").innerHTML = \"OFF\"; document.getElementById(\"inputSwitch1\").checked = dataOFF; } else if(DataJson.TB1 == \"1\") { document.getElementById(\"TB1\").innerHTML = \"ON\"; document.getElementById(\"inputSwitch1\").checked = dataON; } if(DataJson.TB2 == \"0\") { document.getElementById(\"TB2\").innerHTML = \"OFF\"; document.getElementById(\"inputSwitch2\").checked = dataOFF; } else if(DataJson.TB2 == \"1\") { document.getElementById(\"TB2\").innerHTML = \"ON\"; document.getElementById(\"inputSwitch2\").checked = dataON; } if(DataJson.TB3 == \"0\") { document.getElementById(\"TB3\").innerHTML = \"OFF\"; document.getElementById(\"inputSwitch3\").checked = dataOFF; } else if(DataJson.TB3 == \"1\") { document.getElementById(\"TB3\").innerHTML = \"ON\"; document.getElementById(\"inputSwitch3\").checked = dataON; } if(DataJson.TB4 == \"0\") { document.getElementById(\"TB4\").innerHTML = \"OFF\"; document.getElementById(\"inputSwitch4\").checked = dataOFF; } else if(DataJson.TB4 == \"1\") { document.getElementById(\"TB4\").innerHTML = \"ON\"; document.getElementById(\"inputSwitch4\").checked = dataON; } } }; xhttp.open('GET','/Update',true); xhttp.send(); setTimeout(function(){ LoadData() }, 1000); } function DK_DEN() { var TTTB = document.getElementById(\"TB1\").innerHTML; if(TTTB == \"ON\") { var DataSend = \"{-TB1-:-0-}\"; var xhttp = new XMLHttpRequest(); document.getElementById(\"inputSwitch1\").checked = dataOFF; xhttp.open('GET','/Den?'+ DataSend,true); xhttp.send(); } else if(TTTB == \"OFF\") { var DataSend = \"{-TB1-:-1-}\"; var xhttp = new XMLHttpRequest(); document.getElementById(\"inputSwitch1\").checked = dataON; xhttp.open('GET','/Den?'+ DataSend,true); xhttp.send(); } } function DK_QUAT() { var TTTB = document.getElementById(\"TB2\").innerHTML; if(TTTB == \"ON\") { var DataSend = \"{-TB2-:-0-}\"; var xhttp = new XMLHttpRequest(); document.getElementById(\"inputSwitch2\").checked = dataOFF; xhttp.open('GET','/Quat?'+DataSend,true); xhttp.send(); } else if(TTTB == \"OFF\") { var DataSend = \"{-TB2-:-1-}\"; var xhttp = new XMLHttpRequest(); document.getElementById(\"inputSwitch2\").checked = dataON; xhttp.open('GET','/Quat?'+DataSend,true); xhttp.send(); } } function DK_DEN1() { var TTTB = document.getElementById(\"TB3\").innerHTML; if(TTTB == \"ON\") { var DataSend = \"{-TB3-:-0-}\"; var xhttp = new XMLHttpRequest(); document.getElementById(\"inputSwitch3\").checked = dataOFF; xhttp.open('GET','/Den1?'+ DataSend,true); xhttp.send(); } else if(TTTB == \"OFF\") { var DataSend = \"{-TB3-:-1-}\"; var xhttp = new XMLHttpRequest(); document.getElementById(\"inputSwitch3\").checked = dataON; xhttp.open('GET','/Den1?'+ DataSend,true); xhttp.send(); } } function DK_QUAT1() { var TTTB = document.getElementById(\"TB4\").innerHTML; if(TTTB == \"ON\") { var DataSend = \"{-TB4-:-0-}\"; var xhttp = new XMLHttpRequest(); document.getElementById(\"inputSwitch4\").checked = dataOFF; xhttp.open('GET','/Quat1?'+DataSend,true); xhttp.send(); } else if(TTTB == \"OFF\") { var DataSend = \"{-TB4-:-1-}\"; var xhttp = new XMLHttpRequest(); document.getElementById(\"inputSwitch4\").checked = dataON; xhttp.open('GET','/Quat1?'+DataSend,true); xhttp.send(); } } function SendC1() { var TTTB = document.getElementById(\"cai1\").value; var DataSend = \"{-C1-:-\"+TTTB+\"-}\"; var xhttp = new XMLHttpRequest(); xhttp.open('GET','/C1?'+DataSend,true); xhttp.send(); } function SendC2() { var TTTB = document.getElementById(\"cai2\").value; var DataSend = \"{-C2-:-\"+TTTB+\"-}\"; var xhttp = new XMLHttpRequest(); xhttp.open('GET','/C2?'+DataSend,true); xhttp.send(); } function lockpage(){ document.getElementById(\"login\").style.display=\"block\"; document.getElementById(\"hide\").style.display=\"none\"; document.getElementById(\"Username\").value=''; document.getElementById(\"Password\").value=''; } var input = document.getElementById(\"Password\"); input.addEventListener(\"keyup\", function(event) { if (event.keyCode === 13) { document.getElementById(\"myBtn\").click(); } }); var input = document.getElementById(\"Username\"); input.addEventListener(\"keyup\", function(event) { if (event.keyCode === 13) { document.getElementById(\"myBtn\").click(); } }); var correct_User=\"admin\"; var correct_Pass=\"nghia\"; function check_input(){ var inputUsername=document.getElementById(\"Username\").value; var inputPassword=document.getElementById(\"Password\").value; if(inputUsername == correct_User&&inputPassword==correct_Pass){ alert('Đăng nhập thành công'); document.getElementById(\"hide\").style.display=\"block\"; document.getElementById(\"login\").style.display=\"none\"; } else{ alert('Đăng nhập thất bại'); } } </script> </html>";
extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[] asm("_binary_index_html_end");
//const char *webpage ="<!DOCTYPE html> <html lang=\"en\"> <head> <meta charset=\"UTF-8\" /> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /> <title>CSS GRID DASHBOARD</title> <style> body { box-sizing: border-box; font-family: \"Lato\", sans-serif; } .text-primary-p { color: #a5aaad; font-size: 20px; font-weight: 900; } .font-bold { font-weight: 700; } .text-title { color: #2e4a66; } .text-lightblue { color: #469cac; } .text-red { color: #cc3d38; } .text-yellow { color: #a98921; } .text-green { color: #3b9668; } .container { display: grid; height: 100vh; grid-template-columns: 0.8fr 1fr 1fr 1fr; grid-template-rows: 0.2fr 3fr; } main { background: #f3f4f6; grid-area: main; overflow-y: auto; } .main__container { padding: 20px 35px; } .main__title { display: flex; align-items: center; } .main__title > img { max-height: 100px; object-fit: contain; margin-right: 20px; } .main__greeting > h1 { font-size: 24px; color: #2e4a66; margin-bottom: 5px; } .main__greeting > p { font-size: 14px; font-weight: 700; color: #a5aaad; } .main__cards { display: grid; grid-template-columns: 1fr 1fr 1fr 1fr; gap: 30px; margin: 20px 0; } .card { display: flex; flex-direction: column; justify-content: space-around; height: 100px; padding: 35px; border-radius: 8px; background: #ffffff; box-shadow: 5px 5px 13px #ededed, -5px -5px 13px #ffffff; } .card_inner { display: flex; align-items: center; justify-content: space-between; } .card_inner > span { font-size: 25px; } .charts { display: grid; grid-template-columns: 1fr 1fr; gap: 30px; margin-top: 50px; } .charts__left { padding: 25px; border-radius: 5px; background: #ffffff; box-shadow: 5px 5px 13px #ededed, -5px -5px 13px #ffffff; } .charts__left__title { display: flex; align-items: center; justify-content: space-between; } .charts__left__title > div > h1 { font-size: 24px; color: #2e4a66; margin-bottom: 5px; } .charts__left__title > div > p { font-size: 14px; font-weight: 700; color: #a5aaad; } .charts__left__title > i { color: #ffffff; font-size: 20px; background: #ffc100; border-radius: 200px 0px 200px 200px; -moz-border-radius: 200px 0px 200px 200px; -webkit-border-radius: 200px 0px 200px 200px; border: 0px solid #000000; padding: 15px; } .charts__right__cards { display: grid; grid-template-columns: 1fr 1fr; gap: 20px; margin-top: 30px; } .card1 { background: #fddcdf; color: #f65a6f; text-align: center; padding: 25px; border-radius: 5px; font-size: 18px; } .card2 { background: #d6d8d9; color: #3a3e41; text-align: center; padding: 25px; border-radius: 5px; font-size: 18px; } .switch { position: relative; display: inline-block; width: 50px; height: 23px; } .switch input { opacity: 0; width: 0; height: 0; } .slider { position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; -webkit-transition: .4s; transition: .4s; } .slider:before { position: absolute; content: \"\"; height: 16px; width: 16px; left: 4px; bottom: 4px; background-color: white; -webkit-transition: .4s; transition: .4s; } input:checked + .slider { background-color: #2196F3; } input:focus + .slider { box-shadow: 0 0 1px #2196F3; } input:checked + .slider:before { -webkit-transform: translateX(26px); -ms-transform: translateX(26px); transform: translateX(26px); } .slider.round { border-radius: 34px; } .slider.round:before { border-radius: 50%; } </style> </head> <body id=\"body\" onload=\"LoadData()\"> <div class=\"container\"> <main> <div class=\"main__container\"> <div class=\"main__title\"> <img src=\"https://static.vecteezy.com/system/resources/thumbnails/004/607/791/small/man-face-emotive-icon-smiling-male-character-in-blue-shirt-flat-illustration-isolated-on-white-happy-human-psychological-portrait-positive-emotions-user-avatar-for-app-web-design-vector.jpg\" alt=\"\" /> <div class=\"main__greeting\"> <h1>WelCome To My Home</h1> <p>Admin Dashboard</p> </div> </div> <div class=\"main__cards\"> <div class=\"card\"> <img src=\"https://truonglocland.vn/wp-content/uploads/2019/04/icon.png\" height = \"55px\" width=\"55px\" alt=\"\" /> <p class=\"text-primary-p\">Living Room</p> <div class=\"card_inner\"> <label id=\"TB1\">OFF</label> <label class=\"switch\" id=\"switch1\" onclick=\"DK_DEN()\"> <input id=\"inputSwitch1\" type=\"checkbox\" > <span class=\"slider round\"></span> </label> </div> </div> <div class=\"card\"> <img src=\"https://cdn.iconscout.com/icon/free/png-256/cooking-2270962-1890006.png\" height = \"50px\" width=\"50px\" alt=\"\" /> <p class=\"text-primary-p\">Kitchen</p> <div class=\"card_inner\"> <span class=\"font-bold text-title\" id=\"TB2\">OFF</span> <label class=\"switch\" id=\"switch2\" onclick=\"DK_QUAT()\"> <input id=\"inputSwitch2\" type=\"checkbox\" > <span class=\"slider round\"></span> </label> </div> </div> <div class=\"card\"> <img src=\"https://ngocthaolighting.vn/wp-content/uploads/2017/04/fan.png\" height = \"50px\" width=\"50px\" alt=\"\" /> <p class=\"text-primary-p\">Bed Room</p> <div class=\"card_inner\"> <span class=\"font-bold text-title\" id=\"TB3\">OFF</span> <label class=\"switch\" id=\"switch3\" onclick=\"DK_DEN1()\"> <input id=\"inputSwitch3\" type=\"checkbox\" > <span class=\"slider round\"></span> </label> </div> </div> <div class=\"card\"> <img src=\"https://png.pngtree.com/element_our/20190529/ourlarge/pngtree-light-bulb-icon-image_1197512.jpg\" height = \"50px\" width=\"50px\" alt=\"\" /> <p class=\"text-primary-p\">Bath Room</p> <div class=\"card_inner\"> <span class=\"font-bold text-title\" id=\"TB4\">OFF</span> <label class=\"switch\" id=\"switch4\" onclick=\"DK_QUAT1()\"> <input id=\"inputSwitch4\" type=\"checkbox\" > <span class=\"slider round\"></span> </label> </div> </div> </div> <div class=\"charts\"> <div class=\"charts__left\"> <div class=\"charts__left__title\"> <div> <h1>Sensor DHT11</h1> <p>97 Man Thien street, Thu Duc city, HCM city</p> </div> </div> <div class=\"charts__right__cards\"> <div class=\"card1\"> <h1>Temprature</h1> <label id=\"nhietdo\">0</label> <label > &#176;C</label> </div> <div class=\"card2\"> <h1>Humidity</h1> <label id=\"doam\">0</label> <label > % </label> </div> </div> </div> </main> </div> </div> </body> <script> var dataON = !document.getElementById(\"inputSwitch1\").checked; var dataOFF = document.getElementById(\"inputSwitch1\").checked; function LoadData() { var xhttp = new XMLHttpRequest(); xhttp.onreadystatechange = function() { if (this.readyState == 4 && this.status == 200) { var DataVDK = xhttp.responseText; console.log(\"Dữ liệu VDK:\" + DataVDK); var DataJson = JSON.parse(DataVDK); document.getElementById(\"nhietdo\").innerHTML = DataJson.ND; document.getElementById(\"doam\").innerHTML = DataJson.DA; if(DataJson.TB1 == \"0\") { document.getElementById(\"TB1\").innerHTML = \"OFF\"; document.getElementById(\"inputSwitch1\").checked = dataOFF; } else if(DataJson.TB1 == \"1\") { document.getElementById(\"TB1\").innerHTML = \"ON\"; document.getElementById(\"inputSwitch1\").checked = dataON; } if(DataJson.TB2 == \"0\") { document.getElementById(\"TB2\").innerHTML = \"OFF\"; document.getElementById(\"inputSwitch2\").checked = dataOFF; } else if(DataJson.TB2 == \"1\") { document.getElementById(\"TB2\").innerHTML = \"ON\"; document.getElementById(\"inputSwitch2\").checked = dataON; } if(DataJson.TB3 == \"0\") { document.getElementById(\"TB3\").innerHTML = \"OFF\"; document.getElementById(\"inputSwitch3\").checked = dataOFF; } else if(DataJson.TB3 == \"1\") { document.getElementById(\"TB3\").innerHTML = \"ON\"; document.getElementById(\"inputSwitch3\").checked = dataON; } if(DataJson.TB4 == \"0\") { document.getElementById(\"TB4\").innerHTML = \"OFF\"; document.getElementById(\"inputSwitch4\").checked = dataOFF; } else if(DataJson.TB4 == \"1\") { document.getElementById(\"TB4\").innerHTML = \"ON\"; document.getElementById(\"inputSwitch4\").checked = dataON; } } }; xhttp.open('GET','/Update',true); xhttp.send(); setTimeout(function(){LoadData()}, 1000); } function DK_DEN() { var TTTB = document.getElementById(\"TB1\").innerHTML; if(TTTB == \"ON\") { var DataSend = \"{-TB1-:-0-}\"; var xhttp = new XMLHttpRequest(); document.getElementById(\"inputSwitch1\").checked = dataOFF; xhttp.open('GET','/Den?'+ DataSend,true); xhttp.send(); } else if(TTTB == \"OFF\") { var DataSend = \"{-TB1-:-1-}\"; var xhttp = new XMLHttpRequest(); document.getElementById(\"inputSwitch1\").checked = dataON; xhttp.open('GET','/Den?'+ DataSend,true); xhttp.send(); } } function DK_QUAT() { var TTTB = document.getElementById(\"TB2\").innerHTML; if(TTTB == \"ON\") { var DataSend = \"{-TB2-:-0-}\"; var xhttp = new XMLHttpRequest(); document.getElementById(\"inputSwitch2\").checked = dataOFF; xhttp.open('GET','/Quat?'+DataSend,true); xhttp.send(); } else if(TTTB == \"OFF\") { var DataSend = \"{-TB2-:-1-}\"; var xhttp = new XMLHttpRequest(); document.getElementById(\"inputSwitch2\").checked = dataON; xhttp.open('GET','/Quat?'+DataSend,true); xhttp.send(); } } function DK_DEN1() { var TTTB = document.getElementById(\"TB3\").innerHTML; if(TTTB == \"ON\") { var DataSend = \"{-TB3-:-0-}\"; var xhttp = new XMLHttpRequest(); document.getElementById(\"inputSwitch3\").checked = dataOFF; xhttp.open('GET','/Den1?'+ DataSend,true); xhttp.send(); } else if(TTTB == \"OFF\") { var DataSend = \"{-TB3-:-1-}\"; var xhttp = new XMLHttpRequest(); document.getElementById(\"inputSwitch3\").checked = dataON; xhttp.open('GET','/Den1?'+ DataSend,true); xhttp.send(); } } function DK_QUAT1() { var TTTB = document.getElementById(\"TB4\").innerHTML; if(TTTB == \"ON\") { var DataSend = \"{-TB4-:-0-}\"; var xhttp = new XMLHttpRequest(); document.getElementById(\"inputSwitch4\").checked = dataOFF; xhttp.open('GET','/Quat1?'+DataSend,true); xhttp.send(); } else if(TTTB == \"OFF\") { var DataSend = \"{-TB4-:-1-}\"; var xhttp = new XMLHttpRequest(); document.getElementById(\"inputSwitch4\").checked = dataON; xhttp.open('GET','/Quat1?'+DataSend,true); xhttp.send(); } } </script> </html>";
const char *webpage ="<!DOCTYPE html> <html lang=\"en\"> <head> <meta charset=\"UTF-8\" /> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /> <title>CSS GRID DASHBOARD</title> <style> body { box-sizing: border-box; font-family: \"Lato\", sans-serif; } .text-primary-p { color: #a5aaad; font-size: 20px; font-weight: 900; } .font-bold { font-weight: 700; } .text-title { color: #2e4a66; } .text-lightblue { color: #469cac; } .text-red { color: #cc3d38; } .text-yellow { color: #a98921; } .text-green { color: #3b9668; } .container { display: grid; height: 100vh; grid-template-columns: 0.8fr 1fr 1fr 1fr; grid-template-rows: 0.2fr 3fr; } main { background: #f3f4f6; grid-area: main; overflow-y: auto; } .main__container { padding: 20px 35px; } .main__title { display: flex; align-items: center; } .main__title > img { max-height: 100px; object-fit: contain; margin-right: 20px; } .main__greeting > h1 { font-size: 24px; color: #2e4a66; margin-bottom: 5px; } .main__greeting > p { font-size: 14px; font-weight: 700; color: #a5aaad; } .main__cards { display: grid; grid-template-columns: 1fr 1fr 1fr 1fr; gap: 30px; margin: 20px 0; } .card { display: flex; flex-direction: column; justify-content: space-around; height: 100px; padding: 35px; border-radius: 8px; background: #ffffff; box-shadow: 5px 5px 13px #ededed, -5px -5px 13px #ffffff; } .card_inner { display: flex; align-items: center; justify-content: space-between; } .card_inner > span { font-size: 25px; } .charts { display: grid; grid-template-columns: 1fr 1fr; gap: 30px; margin-top: 50px; } .charts__left { padding: 25px; border-radius: 5px; background: #ffffff; box-shadow: 5px 5px 13px #ededed, -5px -5px 13px #ffffff; } .charts__left__title { display: flex; align-items: center; justify-content: space-between; } .charts__left__title > div > h1 { font-size: 24px; color: #2e4a66; margin-bottom: 5px; } .charts__left__title > div > p { font-size: 14px; font-weight: 700; color: #a5aaad; } .charts__left__title > i { color: #ffffff; font-size: 20px; background: #ffc100; border-radius: 200px 0px 200px 200px; -moz-border-radius: 200px 0px 200px 200px; -webkit-border-radius: 200px 0px 200px 200px; border: 0px solid #000000; padding: 15px; } .charts__right__cards { display: grid; grid-template-columns: 1fr 1fr; gap: 20px; margin-top: 30px; } .card1 { background: #fddcdf; color: #f65a6f; text-align: center; padding: 25px; border-radius: 5px; font-size: 18px; } .card2 { background: #d6d8d9; color: #3a3e41; text-align: center; padding: 25px; border-radius: 5px; font-size: 18px; } .switch { position: relative; display: inline-block; width: 50px; height: 23px; } .switch input { opacity: 0; width: 0; height: 0; } .slider { position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; -webkit-transition: .4s; transition: .4s; } .slider:before { position: absolute; content: \"\"; height: 16px; width: 16px; left: 4px; bottom: 4px; background-color: white; -webkit-transition: .4s; transition: .4s; } input:checked + .slider { background-color: #2196F3; } input:focus + .slider { box-shadow: 0 0 1px #2196F3; } input:checked + .slider:before { -webkit-transform: translateX(26px); -ms-transform: translateX(26px); transform: translateX(26px); } .slider.round { border-radius: 34px; } .slider.round:before { border-radius: 50%; } </style> </head> <body id=\"body\" onload=\"LoadData()\"> <div class=\"container\"> <main> <div class=\"main__container\"> <div class=\"main__title\"> <img src=\"https://static.vecteezy.com/system/resources/thumbnails/004/607/791/small/man-face-emotive-icon-smiling-male-character-in-blue-shirt-flat-illustration-isolated-on-white-happy-human-psychological-portrait-positive-emotions-user-avatar-for-app-web-design-vector.jpg\" alt=\"\" /> <div class=\"main__greeting\"> <h1>WelCome To My Home</h1> <p>Admin Dashboard</p> </div> </div> <div class=\"main__cards\"> <div class=\"card\"> <img src=\"https://truonglocland.vn/wp-content/uploads/2019/04/icon.png\" height = \"55px\" width=\"55px\" alt=\"\" /> <p class=\"text-primary-p\">Living Room</p> <div class=\"card_inner\"> <span class=\"font-bold text-title\" id=\"TB1\">OFF</span> <label class=\"switch\" id=\"switch1\" onclick=\"DK_DEN()\"> <input id=\"inputSwitch1\" type=\"checkbox\" > <span class=\"slider round\"></span> </label> </div> </div> <div class=\"card\"> <img src=\"https://cdn.iconscout.com/icon/free/png-256/cooking-2270962-1890006.png\" height = \"50px\" width=\"50px\" alt=\"\" /> <p class=\"text-primary-p\">Kitchen</p> <div class=\"card_inner\"> <span class=\"font-bold text-title\" id=\"TB2\">OFF</span> <label class=\"switch\" id=\"switch2\" onclick=\"DK_QUAT()\"> <input id=\"inputSwitch2\" type=\"checkbox\" > <span class=\"slider round\"></span> </label> </div> </div> <div class=\"card\"> <img src=\"https://ngocthaolighting.vn/wp-content/uploads/2017/04/fan.png\" height = \"50px\" width=\"50px\" alt=\"\" /> <p class=\"text-primary-p\">Bed Room</p> <div class=\"card_inner\"> <span class=\"font-bold text-title\" id=\"TB3\">OFF</span> <label class=\"switch\" id=\"switch3\" onclick=\"DK_DEN1()\"> <input id=\"inputSwitch3\" type=\"checkbox\" > <span class=\"slider round\"></span> </label> </div> </div> <div class=\"card\"> <img src=\"https://png.pngtree.com/element_our/20190529/ourlarge/pngtree-light-bulb-icon-image_1197512.jpg\" height = \"50px\" width=\"50px\" alt=\"\" /> <p class=\"text-primary-p\">Bath Room</p> <div class=\"card_inner\"> <span class=\"font-bold text-title\" id=\"TB4\">OFF</span> <label class=\"switch\" id=\"switch4\" onclick=\"DK_QUAT1()\"> <input id=\"inputSwitch4\" type=\"checkbox\" > <span class=\"slider round\"></span> </label> </div> </div> </div> <div class=\"charts\"> <div class=\"charts__left\"> <div class=\"charts__left__title\"> <div> <h1>Sensor DHT11</h1> <p>97 Man Thien street, Thu Duc city, HCM city</p> </div> </div> <div class=\"charts__right__cards\"> <div class=\"card1\"> <h1>Temprature</h1> <label id=\"nhietdo\">0</label> <label > &#176;C</label> </div> <div class=\"card2\"> <h1>Humidity</h1> <label id=\"doam\">0</label> <label > % </label> </div> </div> </div> </main> </div> </div> </body> <script> var dataON = !document.getElementById(\"inputSwitch1\").checked; var dataOFF = document.getElementById(\"inputSwitch1\").checked; function LoadData() { var xhttp = new XMLHttpRequest(); xhttp.onreadystatechange = function() { if (this.readyState == 4 && this.status == 200) { var DataVDK = xhttp.responseText; console.log(\"Dữ liệu VDK:\" + DataVDK); var DataJson = JSON.parse(DataVDK); document.getElementById(\"nhietdo\").innerHTML = DataJson.ND; document.getElementById(\"doam\").innerHTML = DataJson.DA; if(DataJson.TB1 == \"0\") { document.getElementById(\"TB1\").innerHTML = \"OFF\"; document.getElementById(\"inputSwitch1\").checked = dataOFF; } else if(DataJson.TB1 == \"1\") { document.getElementById(\"TB1\").innerHTML = \"ON\"; document.getElementById(\"inputSwitch1\").checked = dataON; } if(DataJson.TB2 == \"0\") { document.getElementById(\"TB2\").innerHTML = \"OFF\"; document.getElementById(\"inputSwitch2\").checked = dataOFF; } else if(DataJson.TB2 == \"1\") { document.getElementById(\"TB2\").innerHTML = \"ON\"; document.getElementById(\"inputSwitch2\").checked = dataON; } if(DataJson.TB3 == \"0\") { document.getElementById(\"TB3\").innerHTML = \"OFF\"; document.getElementById(\"inputSwitch3\").checked = dataOFF; } else if(DataJson.TB3 == \"1\") { document.getElementById(\"TB3\").innerHTML = \"ON\"; document.getElementById(\"inputSwitch3\").checked = dataON; } if(DataJson.TB4 == \"0\") { document.getElementById(\"TB4\").innerHTML = \"OFF\"; document.getElementById(\"inputSwitch4\").checked = dataOFF; } else if(DataJson.TB4 == \"1\") { document.getElementById(\"TB4\").innerHTML = \"ON\"; document.getElementById(\"inputSwitch4\").checked = dataON; } } }; xhttp.open('GET','/Update',true); xhttp.send(); setTimeout(function(){LoadData()}, 1000); } function DK_DEN() { var TTTB = document.getElementById(\"TB1\").innerHTML; if(TTTB == \"ON\") { var DataSend = \"{-TB1-:-0-}\"; var xhttp = new XMLHttpRequest(); document.getElementById(\"inputSwitch1\").checked = dataOFF; xhttp.open('GET','/Den?'+ DataSend,true); xhttp.send(); } else if(TTTB == \"OFF\") { var DataSend = \"{-TB1-:-1-}\"; var xhttp = new XMLHttpRequest(); document.getElementById(\"inputSwitch1\").checked = dataON; xhttp.open('GET','/Den?'+ DataSend,true); xhttp.send(); } } function DK_QUAT() { var TTTB = document.getElementById(\"TB2\").innerHTML; if(TTTB == \"ON\") { var DataSend = \"{-TB2-:-0-}\"; var xhttp = new XMLHttpRequest(); document.getElementById(\"inputSwitch2\").checked = dataOFF; xhttp.open('GET','/Quat?'+DataSend,true); xhttp.send(); } else if(TTTB == \"OFF\") { var DataSend = \"{-TB2-:-1-}\"; var xhttp = new XMLHttpRequest(); document.getElementById(\"inputSwitch2\").checked = dataON; xhttp.open('GET','/Quat?'+DataSend,true); xhttp.send(); } } function DK_DEN1() { var TTTB = document.getElementById(\"TB3\").innerHTML; if(TTTB == \"ON\") { var DataSend = \"{-TB3-:-0-}\"; var xhttp = new XMLHttpRequest(); document.getElementById(\"inputSwitch3\").checked = dataOFF; xhttp.open('GET','/Den1?'+ DataSend,true); xhttp.send(); } else if(TTTB == \"OFF\") { var DataSend = \"{-TB3-:-1-}\"; var xhttp = new XMLHttpRequest(); document.getElementById(\"inputSwitch3\").checked = dataON; xhttp.open('GET','/Den1?'+ DataSend,true); xhttp.send(); } } function DK_QUAT1() { var TTTB = document.getElementById(\"TB4\").innerHTML; if(TTTB == \"ON\") { var DataSend = \"{-TB4-:-0-}\"; var xhttp = new XMLHttpRequest(); document.getElementById(\"inputSwitch4\").checked = dataOFF; xhttp.open('GET','/Quat1?'+DataSend,true); xhttp.send(); } else if(TTTB == \"OFF\") { var DataSend = \"{-TB4-:-1-}\"; var xhttp = new XMLHttpRequest(); document.getElementById(\"inputSwitch4\").checked = dataON; xhttp.open('GET','/Quat1?'+DataSend,true); xhttp.send(); } } </script> </html>";

void delay(uint32_t time){
	vTaskDelay(time / portTICK_PERIOD_MS);
}

void DataJson(unsigned int ND ,unsigned int DA, unsigned int TB1, unsigned int TB2, unsigned int TB3,  unsigned int TB4, unsigned int C1, unsigned int C2){
	//https://programmersought.com/article/98966009006/
	char JSON[100] = "";
	char Str_ND[10] = ""; 
	char Str_DA[10] = "";
	char Str_TB1[10] = "";
	char Str_TB2[10] = "";
	char Str_TB3[10] = "";
	char Str_TB4[10] = "";
	char Str_C1[10] = "";
	char Str_C2[10] = "";
	for (int i = 0; i < 100; i++){
		JSON1[i] = 0;
	}
	sprintf(Str_ND, "%d", ND); // đưa dữ liệu mảng số về dạng string
	sprintf(Str_DA, "%d", DA);
	sprintf(Str_TB1, "%d", TB1);
	sprintf(Str_TB2, "%d", TB2);
	sprintf(Str_TB3, "%d", TB3);
	sprintf(Str_TB4, "%d", TB4);
	sprintf(Str_C2, "%d", C2);
	sprintf(Str_C1, "%d", C1);
	//  ghép chuỗi dữ liệu 
	strcat(JSON, "{\"ND\":\"");
	strcat(JSON, Str_ND); strcat(JSON, "\",");
	strcat(JSON, "\"DA\":\"");
	strcat(JSON, Str_DA); strcat(JSON, "\",");

	strcat(JSON, "\"TB1\":\"");
	strcat(JSON, Str_TB1); strcat(JSON, "\",");
	strcat(JSON, "\"TB2\":\"");
	strcat(JSON, Str_TB2); strcat(JSON, "\",");
	strcat(JSON, "\"TB3\":\"");
	strcat(JSON, Str_TB3); strcat(JSON, "\",");
	strcat(JSON, "\"TB4\":\"");
	strcat(JSON, Str_TB4); strcat(JSON, "\",");
	strcat(JSON, "\"C1\":\"");
	strcat(JSON, Str_C1); strcat(JSON, "\",");
	strcat(JSON, "\"C2\":\"");
	strcat(JSON, Str_C2); strcat(JSON, "\"}");
	strcat(JSON, "\r\n");
	printf("DataJson: %s\n", JSON);
	sprintf(JSON1, "%s", JSON);
	uart_write_bytes(UART_NUM_1, JSON, strlen(JSON)); // write data uart 1
}
void read_dht11(){
    printf("Temperature is %d \n", DHT11_read().temperature);
    printf("Humidity is %d\n", DHT11_read().humidity);
	if(DHT11_read().temperature > -1 && DHT11_read().humidity > -1){
		nhietdo = DHT11_read().temperature;
		doam = DHT11_read().humidity;
	}
}
//void check_button1(gpio_num_t gpio_num);

void check_button(){
	if (gpio_get_level(NUT1) == 0){
		vTaskDelay(20 / portTICK_PERIOD_MS);
		if(gpio_get_level(NUT1) == 0){
			if (TB1 == 0){
				gpio_set_level(DEN, 1);
				TB1 = 1;
				printf("ON 1!\n");
			}
			else {
				gpio_set_level(DEN, 0);
				TB1 = 0;
				printf("OFF 1!\n");
			}
			while(gpio_get_level(NUT1) == 0){
				vTaskDelay(20 / portTICK_PERIOD_MS);
			}
		}
	}
	if(gpio_get_level(NUT2) == 0){
		vTaskDelay(20 / portTICK_PERIOD_MS);
		if(gpio_get_level(NUT2) == 0){
			if (TB2 == 0){
				gpio_set_level(QUAT, 1);
				TB2 = 1;
				printf("ON 2!\n");
			}
			else {
				gpio_set_level(QUAT, 0);
				TB2 = 0;
				printf("OFF 2!\n");
			}
			while(gpio_get_level(NUT2) == 0){
				vTaskDelay(20 / portTICK_PERIOD_MS);
			}
		}
	}

	if(gpio_get_level(NUT3) == 0){
		vTaskDelay(20 / portTICK_PERIOD_MS);
		if(gpio_get_level(NUT3) == 0){
			if (TB3 == 0){
				gpio_set_level(QUAT1, 1);
				TB3 = 1;
				printf("ON 3!\n");
			}
			else {
				gpio_set_level(QUAT1, 0);
				TB3 = 0;
				printf("OFF 3!\n");
			}
			while(gpio_get_level(NUT3) == 0){
				vTaskDelay(20 / portTICK_PERIOD_MS);
			}
		}
	}
	if(gpio_get_level(NUT4) == 0){
		vTaskDelay(20 / portTICK_PERIOD_MS);
		if(gpio_get_level(NUT4) == 0){
			if (TB4 == 0){
				gpio_set_level(DEN1, 1);
				TB4 = 1;
				printf("ON 4\n");
			}
			else {
				gpio_set_level(DEN1, 0);
				TB4 = 0;
				printf("OFF 4!\n");
			}
			while(gpio_get_level(NUT4) == 0){
				vTaskDelay(20 / portTICK_PERIOD_MS);
			}
		}
	}
}

void functionSendData(void *arg){
	while (1){
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
	vTaskDelete(NULL);
}
void functionButton(void *arg){ // đưa hàm set chân vào function
	// set output pin 
    gpio_reset_pin(DEN);
    gpio_set_direction(DEN, GPIO_MODE_OUTPUT);
	gpio_set_level(DEN, 0);
    gpio_reset_pin(QUAT);
    gpio_set_direction(QUAT, GPIO_MODE_OUTPUT);
	gpio_set_level(QUAT, 0);

	gpio_reset_pin(DEN1);
    gpio_set_direction(DEN1, GPIO_MODE_OUTPUT);
	gpio_set_level(DEN1, 0);
    gpio_reset_pin(QUAT1);
    gpio_set_direction(QUAT1, GPIO_MODE_OUTPUT);
	gpio_set_level(QUAT1, 0);
	// set input pin 

	gpio_reset_pin(NUT1);
    gpio_set_direction(NUT1, GPIO_MODE_INPUT);
	gpio_set_pull_mode(NUT1, GPIO_PULLUP_ONLY);
    gpio_reset_pin(NUT2);
    gpio_set_direction(NUT2, GPIO_MODE_INPUT);
	gpio_set_pull_mode(NUT2, GPIO_PULLUP_ONLY);
	
	gpio_reset_pin(NUT3);
    gpio_set_direction(NUT3, GPIO_MODE_INPUT);
	gpio_set_pull_mode(NUT3, GPIO_PULLUP_ONLY);
    gpio_reset_pin(NUT4);
    gpio_set_direction(NUT4, GPIO_MODE_INPUT);
	gpio_set_pull_mode(NUT4, GPIO_PULLUP_ONLY);

	while(1){
		check_button(); 
		vTaskDelay(50 / portTICK_PERIOD_MS);
	}
	vTaskDelete(NULL);
}
void functionADC(void *arg){ // ADC 1 va ADC 2(su dung cho wifi)

	adc1_config_width(ADC_WIDTH_BIT_12); // cấu hình adc
	adc1_config_channel_atten(ADC1_CHANNEL_4, ADC_ATTEN_DB_0);
	while(1){
		C2 = adc1_get_raw(ADC1_CHANNEL_4);
		printf("Analog: %d\n", C2);
		if( nhietdo > C1)
		{
			printf("nhiet do Lon hon");
		}
		vTaskDelay(2000 / portTICK_PERIOD_MS);
	}
	vTaskDelete(NULL);
}// ADC1_0 -> 36 | ADC1_3 -> 39 | ADC1_4 -> 32 | ADC1_5 -> 33 | ADC1_6 -> 34 | ADC1_7 -> 35
void initUart(){
	const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE, // mặc địn
        .source_clk = UART_SCLK_APB, // mặc định
    };

	uart_driver_install(UART_NUM_1, RX_BUF_SIZE * 2, 0, 0, NULL, 0);	
    uart_param_config(UART_NUM_1, &uart_config); // uart config
    uart_set_pin(UART_NUM_1, TXD2, RXD2, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE); // set uart pin
}

void functionTransmittingUart(void *arg){ 

	while(1){
		read_dht11();
		DataJson(nhietdo , doam, TB1, TB2, TB3, TB4, C1, C2);		
		vTaskDelay(2000 / portTICK_PERIOD_MS);
	}
	vTaskDelete(NULL);
}
void functionReceiveUart(void *arg){ //https://embetronicx.com/tutorials/wireless/esp32/idf/esp32-idf-serial-communication-tutorial/
	uint8_t* data = (uint8_t*) malloc(RX_BUF_SIZE+1); // ?? buffer size của data
	//Hàm malloc trả về một con trỏ tới bộ nhớ đã cấp phát
	while(1){
		const int rxBytes = uart_read_bytes(UART_NUM_1, data, RX_BUF_SIZE, 500 / portTICK_RATE_MS);// length of data
		// dọc data uart 1, data, size, thời gian time out
		if (rxBytes > 0) {
			//data[rxBytes] = 0;	// phần tử cuối cùng = null		
			printf("Data nhan duoc: %s\n", data);
			
			//só lượng  phan tu, kich thươc
			char *bufData = calloc(rxBytes+1, sizeof(data)); //cấp phát bộ nhớ được yêu cầu và trả về một con trỏ tới buf
			snprintf(bufData, rxBytes+1,  "%s", (char *)data);
			printf("bufData: %s\n", bufData);
			str_json = cJSON_Parse(bufData);
			//Nếu str_json trả về False lỗi dữ liệu JSON
			bzero(data, RX_BUF_SIZE); // xóa vùng nhớ trong C
			if(!str_json){
				printf("Data JSON ERROR!!!\n");
			}
			else{
				printf("Data JSON OK!!!\n");
				if(cJSON_GetObjectItem(str_json, "TB1")){
					// strstr
					if(strstr( cJSON_GetObjectItem(str_json, "TB1")->valuestring,"0") != NULL ){
						gpio_set_level(DEN, 0); // kiem tra có tồn tại "0"
						TB1 = 0;
						printf("OFF 1!!!\n");
					}
					else if(strstr( cJSON_GetObjectItem(str_json, "TB1")->valuestring,"1") != NULL ){
						gpio_set_level(DEN, 1);
						TB1 = 1;
						printf("ON 1!!!\n");
					}					
				}
				if(cJSON_GetObjectItem(str_json, "TB2")){
					// strstr tìm kiếm "0" trong valuestring
					//hàm trả về một con trỏ null nếu "TB2"  không có trong TB2 -> valuestring.
					if(strstr( cJSON_GetObjectItem(str_json, "TB2")->valuestring,"0") != NULL ){
						gpio_set_level(QUAT, 0);
						TB2 = 0;
						printf("OFF 2!!!\n");
					}
					else if(strstr( cJSON_GetObjectItem(str_json, "TB2")->valuestring,"1") != NULL ){
						gpio_set_level(QUAT, 1);
						TB2 = 1;
						printf("ON 2!!!\n");
					}					
				}
				if(cJSON_GetObjectItem(str_json, "TB3")){ // tb3
					if(strstr( cJSON_GetObjectItem(str_json, "TB3")->valuestring,"0") != NULL ){
						gpio_set_level(DEN1, 0);
						TB3 = 0;
						printf("OFF 3!!!\n");
					}
					else if(strstr( cJSON_GetObjectItem(str_json, "TB3")->valuestring,"1") != NULL ){
						gpio_set_level(DEN1, 1);
						TB3 = 1;
						printf("ON 3!!!\n");
					}					
				}
				if(cJSON_GetObjectItem(str_json, "TB4")){ //tb4
					if(strstr( cJSON_GetObjectItem(str_json, "TB4")->valuestring,"0") != NULL ){
						gpio_set_level(QUAT1, 0);
						TB4 = 0;
						printf("OFF 4!!!\n");
					}
					else if(strstr( cJSON_GetObjectItem(str_json, "TB4")->valuestring,"1") != NULL ){
						gpio_set_level(QUAT1, 1);
						TB4 = 1;
						printf("ON 4!!!\n");
					}					
				}
				if(cJSON_GetObjectItem(str_json, "C2")){
					C2 = atoi(cJSON_GetObjectItem(str_json, "C2")->valuestring); // atoi convert string to interger	
					printf("OK C2: %d\r\n",C2);					
				}
				if(cJSON_GetObjectItem(str_json, "C1")){
					C1 = atoi(cJSON_GetObjectItem(str_json, "C1")->valuestring); // atoi convert string to interger	
					printf("OK C1: %d\r\n",C1);					
				}

			}
			cJSON_Delete(str_json); // xóa mảng json
			free(bufData);
		}
		vTaskDelay(50 / portTICK_PERIOD_MS);
	}
	vTaskDelete(NULL);
}


static esp_err_t adder_get_handler(httpd_req_t *req)// webpage
{
	printf("Send Webpage!!!\n");
	httpd_resp_set_type(req, "text/html");
	httpd_resp_send(req, webpage, HTTPD_RESP_USE_STRLEN); // send webpage, strlen
    //httpd_resp_send(req, (const char *)index_html_start, index_html_start - index_html_end);
	return ESP_OK;
}
static const httpd_uri_t adder_get = {// hàm khởi tạo web
    .uri      = "/",
    .method   = HTTP_GET,
    .handler  = adder_get_handler,
    .user_ctx = NULL
};
static esp_err_t adder_get_handlerUpdate(httpd_req_t *req)// ham send data
{
   // printf("Update data!!!\n");
	DataJson(nhietdo, doam, TB1, TB2, TB3, TB4, C1, C2);
	httpd_resp_send(req, JSON1, HTTPD_RESP_USE_STRLEN);// JSON
    return ESP_OK;
}
static const httpd_uri_t adder_getUpdate = {
    .uri      = "/Update", // duowng dan
    .method   = HTTP_GET,// phuowng thuc
    .handler  = adder_get_handlerUpdate, // su kien
    .user_ctx = NULL
};
void JsonParse(char *buf)
{
	str_json = cJSON_Parse(buf);
	if(!str_json){
		printf("Data JSON ERROR!!!\n");
	}
	else{
		printf("Data JSON OK!!!\n");
		if(cJSON_GetObjectItem(str_json, "TB1")){
			// strstr
			if(strstr( cJSON_GetObjectItem(str_json, "TB1")->valuestring,"0") != NULL ){
				gpio_set_level(DEN, 0); // kiem tra có tồn tại "0"
				TB1 = 0;
				printf("OFF 1!!!\n");
			}
			else if(strstr( cJSON_GetObjectItem(str_json, "TB1")->valuestring,"1") != NULL ){
				gpio_set_level(DEN, 1);
				TB1 = 1;
				printf("ON 1!!!\n");
			}					
		}
		if(cJSON_GetObjectItem(str_json, "TB2")){
			// strstr tìm kiếm "0" trong valuestring
			//hàm trả về một con trỏ null nếu "TB2"  không có trong TB2 -> valuestring.
			if(strstr( cJSON_GetObjectItem(str_json, "TB2")->valuestring,"0") != NULL ){
				gpio_set_level(QUAT, 0);
				TB2 = 0;
				printf("OFF 2!!!\n");
			}
			else if(strstr( cJSON_GetObjectItem(str_json, "TB2")->valuestring,"1") != NULL ){
				gpio_set_level(QUAT, 1);
				TB2 = 1;
				printf("ON 2!!!\n");
			}					
		}

		if(cJSON_GetObjectItem(str_json, "TB3")){
			// strstr
			if(strstr( cJSON_GetObjectItem(str_json, "TB3")->valuestring,"0") != NULL ){
				gpio_set_level(DEN1, 0); // kiem tra có tồn tại "0"
				TB3 = 0;
				printf("OFF 3!!!\n");
			}
			else if(strstr( cJSON_GetObjectItem(str_json, "TB3")->valuestring,"1") != NULL ){
				gpio_set_level(DEN1, 1);
				TB3 = 1;
				printf("ON 3!!!\n");
			}					
		}
		if(cJSON_GetObjectItem(str_json, "TB4")){
			// strstr
			if(strstr( cJSON_GetObjectItem(str_json, "TB4")->valuestring,"0") != NULL ){
				gpio_set_level(QUAT1, 0); // kiem tra có tồn tại "0"
				TB4 = 0;
				printf("OFF 4!!!\n");
			}
			else if(strstr( cJSON_GetObjectItem(str_json, "TB4")->valuestring,"1") != NULL ){
				gpio_set_level(QUAT1, 1);
				TB4 = 1;
				printf("ON 4!!!\n");
			}					
		}
		if(cJSON_GetObjectItem(str_json, "C2")){
			C2 = atoi(cJSON_GetObjectItem(str_json, "C2")->valuestring); // atoi convert string to interger	
			printf("OK C2: %d\r\n",C2);					
		}
		if(cJSON_GetObjectItem(str_json, "C1")){
			C1 = atoi(cJSON_GetObjectItem(str_json, "C1")->valuestring); // atoi convert string to interger	
			printf("OK C1: %d\r\n",C1);					
		}
	}
	cJSON_Delete(str_json); // xóa mảng json
}
static esp_err_t adder_get_Den(httpd_req_t *req)
{	
	printf("Run thiết bị 1!!!\n");
	char*  buf;
    size_t buf_len;

    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1) {
        buf = (char*)malloc(buf_len);
        if(!buf){
            httpd_resp_send_500(req);
            return ESP_FAIL;
        }
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
			printf("buf Den: %s\n", buf); 
			for(int i = 0 ; i < strlen(buf); i++){
				if(buf[i] == '-'){
					buf[i] = '"';
				}
			}
			printf("buf Den Fix: %s\n", buf); 
			JsonParse(buf);	
        } 
		else {
            free(buf);
            httpd_resp_send_404(req);
            return ESP_FAIL;
        }
        free(buf);
    } 
	else 
	{
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }
	httpd_resp_send(req, webpage, HTTPD_RESP_USE_STRLEN);
	//httpd_resp_send(req, (const char *)index_html_start, index_html_start - index_html_end);

    return ESP_OK;
}
static esp_err_t adder_get_Den1(httpd_req_t *req)
{	
	printf("Run thiết bị 3!!!\n");
	char*  buf;
    size_t buf_len;

    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1) {
        buf = (char*)malloc(buf_len);
        if(!buf){
            httpd_resp_send_500(req);
            return ESP_FAIL;
        }
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
			printf("buf Den: %s\n", buf); 
			for(int i = 0 ; i < strlen(buf); i++){
				if(buf[i] == '-'){
					buf[i] = '"';
				}
			}
			printf("buf Den Fix: %s\n", buf); 
			JsonParse(buf);	
        } 
		else {
            free(buf);
            httpd_resp_send_404(req);
            return ESP_FAIL;
        }
        free(buf);
    } 
	else 
	{
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }
	httpd_resp_send(req, webpage, HTTPD_RESP_USE_STRLEN);
	//httpd_resp_send(req, (const char *)index_html_start, index_html_start - index_html_end);
    return ESP_OK;
}
static const httpd_uri_t adder_Den = {
    .uri      = "/Den", // duowng dan
    .method   = HTTP_GET,// phuowng thuc
    .handler  = adder_get_Den, // su kien
    .user_ctx = NULL
};
static const httpd_uri_t adder_Den1 = {
    .uri      = "/Den1", // duowng dan
    .method   = HTTP_GET,// phuowng thuc
    .handler  = adder_get_Den1, // su kien
    .user_ctx = NULL
};
static esp_err_t adder_get_handlerC2(httpd_req_t *req)
{
	printf("Run C2!!!\n");
	char*  buf;
    size_t buf_len;
    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1) 
	{
        buf = (char*)malloc(buf_len);
        if(!buf)
		{
            httpd_resp_send_500(req);
            return ESP_FAIL;
        }
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) 
		{
			printf("buf C2: %s\n", buf); 
						
			for(int i = 0 ; i < strlen(buf); i++)
			{
				if(buf[i] == '-')
				{
					buf[i] = '"';
				}
			}
			
			printf("buf C2 Fix: %s\n", buf); 
			JsonParse(buf);
        } 
		else 
		{
            free(buf);
            httpd_resp_send_404(req);
            return ESP_FAIL;
        }
        free(buf);
    } 
	else 
	{
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }
	httpd_resp_send(req, webpage, HTTPD_RESP_USE_STRLEN);
	//httpd_resp_send(req, (const char *)index_html_start, index_html_start - index_html_end);

    return ESP_OK;
}
static esp_err_t adder_get_handlerC1(httpd_req_t *req)
{
	printf("Run C1!!!\n");
	char*  buf;
    size_t buf_len;

    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1) 
	{
        buf = (char*)malloc(buf_len);
        if(!buf)
		{
            httpd_resp_send_500(req);
            return ESP_FAIL;
        }
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) 
		{
			printf("buf C1: %s\n", buf); 
						
			for(int i = 0 ; i < strlen(buf); i++)
			{
				if(buf[i] == '-')
				{
					buf[i] = '"';
				}
			}
			
			printf("buf C1 Fix: %s\n", buf); 
			JsonParse(buf);
        } 
		else 
		{
            free(buf);
            httpd_resp_send_404(req);
            return ESP_FAIL;
        }
        free(buf);
    } 
	else 
	{
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }
	httpd_resp_send(req, webpage, HTTPD_RESP_USE_STRLEN);
	    //httpd_resp_send(req, (const char *)index_html_start, index_html_start - index_html_end);

    return ESP_OK;
}
static const httpd_uri_t adder_getC1 = {
    .uri      = "/C1",
    .method   = HTTP_GET,
    .handler  = adder_get_handlerC1,
    .user_ctx = NULL
};
static const httpd_uri_t adder_getC2 = {
    .uri      = "/C2",
    .method   = HTTP_GET,
    .handler  = adder_get_handlerC2,
    .user_ctx = NULL
};
static esp_err_t adder_get_Quat(httpd_req_t *req)
{	
	printf("Run thiết bị 2!!!\n");
	char*  buf;
    size_t buf_len;

    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1) {
        buf = (char*)malloc(buf_len);
        if(!buf){
            httpd_resp_send_500(req);
            return ESP_FAIL;
        }
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
			printf("buf Quat: %s\n", buf); 
			for(int i = 0 ; i < strlen(buf); i++){
				if(buf[i] == '-'){
					buf[i] = '"';
				}
			}
			printf("buf Den Fix: %s\n", buf); 
			JsonParse(buf);	
        } 
		else {
            free(buf);
            httpd_resp_send_404(req);
            return ESP_FAIL;
        }
        free(buf);
    } 
	else 
	{
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }
	httpd_resp_send(req, webpage, HTTPD_RESP_USE_STRLEN);
	   // httpd_resp_send(req, (const char *)index_html_start, index_html_start - index_html_end);

    return ESP_OK;
}
static esp_err_t adder_get_Quat1(httpd_req_t *req)
{	
	printf("Run thiết bị 3!!!\n");
	char*  buf;
    size_t buf_len;

    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1) {
        buf = (char*)malloc(buf_len);
        if(!buf){
            httpd_resp_send_500(req);
            return ESP_FAIL;
        }
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
			printf("buf Quat: %s\n", buf); 
			for(int i = 0 ; i < strlen(buf); i++){
				if(buf[i] == '-'){
					buf[i] = '"';
				}
			}
			printf("buf Den Fix: %s\n", buf); 
			JsonParse(buf);	
        } 
		else {
            free(buf);
            httpd_resp_send_404(req);
            return ESP_FAIL;
        }
        free(buf);
    } 
	else 
	{
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }
	
	httpd_resp_send(req, webpage, HTTPD_RESP_USE_STRLEN);
	//httpd_resp_send(req, (const char *)index_html_start, index_html_start - index_html_end);

    return ESP_OK;
}
static const httpd_uri_t adder_Quat = {
    .uri      = "/Quat", // duowng dan
    .method   = HTTP_GET,// phuowng thuc
    .handler  = adder_get_Quat, // su kien
    .user_ctx = NULL
};
static const httpd_uri_t adder_Quat1 = {
    .uri      = "/Quat1", // duowng dan
    .method   = HTTP_GET,// phuowng thuc
    .handler  = adder_get_Quat1, // su kien
    .user_ctx = NULL
};
static httpd_handle_t start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    httpd_handle_t server;

    if (httpd_start(&server, &config) == ESP_OK) 
	{
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &adder_get);
		httpd_register_uri_handler(server, &adder_getUpdate);
		httpd_register_uri_handler(server, &adder_Den);
		httpd_register_uri_handler(server, &adder_Quat);

		httpd_register_uri_handler(server, &adder_Den1);
		httpd_register_uri_handler(server, &adder_Quat1);

		httpd_register_uri_handler(server, &adder_getC1);
		httpd_register_uri_handler(server, &adder_getC2);
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

// connect wifi
static void event_handler(void* arg, esp_event_base_t event_base,int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) 
	{
        esp_wifi_connect();
    } 
	else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) 
	{
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) 
		{
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG1, "retry to connect to the AP");
        } 
		else 
		{
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG1,"connect to the AP fail");
    } 
	else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) 
	{
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG1, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}
void ConnectWiFi(void)
{
	s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .password = EXAMPLE_ESP_WIFI_PASS,
          
			.threshold.authmode = WIFI_AUTH_WPA2_PSK,

            .pmf_cfg = {
                .capable = true,
                .required = false
            },
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG1, "wifi_init_sta finished.");

    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);
    if (bits & WIFI_CONNECTED_BIT) 
	{
        ESP_LOGI(TAG1, "connected to ap SSID:%s password:%s",EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    } 
	else if (bits & WIFI_FAIL_BIT) 
	{
        ESP_LOGI(TAG1, "Failed to connect to SSID:%s, password:%s",EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    }
	else 
	{
        ESP_LOGE(TAG1, "UNEXPECTED EVENT");
    }


    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
    vEventGroupDelete(s_wifi_event_group);
}
void app_main(void)
{
	esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
	
	ConnectWiFi();	

	delay(500);
	server = start_webserver();

    initUart();
	DHT11_init(GPIO_NUM_18);
	xTaskCreate(functionSendData, "task1", 2048*2, NULL, 3, NULL);
	xTaskCreate(functionButton, "task2", 2048*2, NULL, 3, NULL);
	xTaskCreate(functionADC, "task4", 2048*2, NULL, 3, NULL);
	//https://www.freertos.org/a00125.html
	xTaskCreate(functionTransmittingUart, "uart_tx", 2048*8, NULL, 3, NULL);
	xTaskCreate(functionReceiveUart, "uart_rx", 2048*2, NULL, 4, NULL);
}





