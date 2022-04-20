#include <Arduino.h> 
#include <WiFi.h> 
#include <HTTPClient.h>
#include "cJSON.h"
#include<ArduinoJson.h>

#define NTP1  "ntp1.aliyun.com"
#define NTP2  "ntp2.aliyun.com"
#define NTP3  "ntp3.aliyun.com"
String key="";


struct ncov_data{         //该结构体用于保存数据
    long currentConfirmedCount;//现存确诊
    long currentConfirmedIncr;//较昨日
    long confirmedCount;//累计确诊
    long confirmedIncr;//较昨日
    long curedCount;//累计治愈
    long curedIncr;//较昨日
    long seriousCount;//现存无症状
    long seriousIncr;//较昨日
    long deadCount;//累计死亡
    long deadIncr;//较昨日
    String updateTime;//更新时间
};

void printLocalWeather(){
    HTTPClient http2;
    http2.begin("http://www.weather.com.cn/data/cityinfo/101120201.html");
    int httpCode = http2.GET();
    if(httpCode == HTTP_CODE_OK){
      String pageData = http2.getString();
      //Serial.println(pageData);
      DynamicJsonDocument doc1(1000);
      deserializeJson(doc1,pageData);
      JsonObject obj = doc1.as<JsonObject>();
      String weatherInfo = obj["weatherinfo"];
      deserializeJson(doc1,weatherInfo);
      JsonObject obj1 = doc1.as<JsonObject>();
      String city = obj1["city"];
      String temp1 = obj1["temp1"];
      String temp2 = obj1["temp2"];
      String weather = obj1["weather"];
      String cityInfo ="地点："+ city;
      String tempInfo =" 温度: " + temp1 + "~" + temp2;
      String cityWeatherinfo = " 天气状况: " + weather;
      Serial.println("获得天气情况如下：");
      Serial.print(cityInfo);
      Serial.print(tempInfo);
      Serial.println(cityWeatherinfo);
    }else{
      Serial.println("GET ERR");
    }
    http2.end();
}
void setup()
{
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin("SSID","password");
  while (WiFi.status() != WL_CONNECTED)  
  {
    delay(500);
    Serial.print(".");
  }
  
  Serial.print("WiFi connnected!\n");
}
void loop() {
  HTTPClient http;     //建立HTTPClient对象
  HTTPClient wechat;
 
  http.begin("https://lab.isaaclin.cn/nCoV/api/overall");//向该网址发送http数据请求
  int httpCode = http.GET(); 
  if (httpCode > 0)
  {
  if (httpCode == HTTP_CODE_OK)
  {
  String payload = http.getString();
  Serial.println(payload);   
  ncov_data China_data; 
  DynamicJsonDocument doc(1000);
  deserializeJson(doc, payload);     //反序列化数据
  JsonObject results_0 = doc["results"][0];
   //将解析出的数据赋值给结构体
      China_data.currentConfirmedCount = results_0["currentConfirmedCount"].as<int>();
      China_data.currentConfirmedIncr = results_0["currentConfirmedIncr"].as<int>();
      China_data.confirmedCount = results_0["confirmedCount"].as<int>();
      China_data.confirmedIncr = results_0["confirmedIncr"].as<int>();
      China_data.curedCount = results_0["curedCount"].as<int>();
      China_data.curedIncr = results_0["curedIncr"].as<int>();
      China_data.deadCount = results_0["deadCount"].as<int>();
      China_data.deadIncr = results_0["deadIncr"].as<int>();
      China_data.seriousCount = results_0["seriousCount"].as<int>();
      China_data.seriousIncr = results_0["seriousIncr"].as<int>();
      Serial.println("");
      Serial.println("----------------国内疫情---------------");
      Serial.print("现存确诊: ");Serial.println(China_data.currentConfirmedCount);
      Serial.print("累计确诊: ");Serial.println(China_data.confirmedCount);
      Serial.print("累计治愈: ");Serial.println(China_data.curedCount);
      Serial.print("现存无症: ");Serial.println(China_data.seriousCount);
      Serial.print("累计死亡: ");Serial.println(China_data.deadCount);
      String title="现存确诊";
      long message=China_data.currentConfirmedCount;
      wechat.begin("http://sc.ftqq.com/"+key+".send?text="+title+"&desp=现存确诊病例数"+message);
      int httpCode=wechat.GET();
      wechat.end();
      
  
  }
  else
  {
    Serial.print("Api接口可能存在服务器问题，请稍后再试");
  }
}
http.end(); // 结束当前连接

printLocalWeather();
delay(120000);//两分钟发送一次请求，过于频繁可能导致ip被接口封掉

}
