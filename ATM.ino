#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>   
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <ThingSpeak.h>
#include <HTTPClient.h>


#define EEPROM_SIZE 3


// WIFI Credentials
const char* ssid = " wifiName";
const char* password = "wifiPassword";

// Initialize Telegram BOT
#define BOTtoken "2111371073:AAHhobsR9gpcYG0nxdndbixnvlelwnxc"  // replace this with your bot token
#define CHAT_ID "1116287546"  //replace with your telegram user ID

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Bot checks for message with a time interal of 1 secs.
int bot_delay = 1000;
unsigned long lastTimeBotRan;
int BIG_BALANCE = 25000;
int denom2k = 5;
int denom1k = 10;
int denom500 = 10;

// Credentials of ThingSpeak
unsigned long myChannelNumber = enterchanmel_num123;
String myWriteAPIKey = " enter11digitAPIAKey";
const char* serverName = "https://api.thingspeak.com/update";

// Function that handles the options and their actions as given in the bot by user
int handleNewMessages(int numNewMessages) {
  int d1,d2,r1,r2,r3,r4;  //d1,d2 are tens and units digit of OTP , r1,r2,r3,r4 are the thousands,hundreds,tens and unit digit of withdrawal amount
  String otp_typd;
  Serial.println("Handling New Transaction");
  Serial.println(String(numNewMessages));
  long otp = random(0,100);   // this generated random numbers from 0 to 99 both inclusive as long integers
  String otp_final;

  //Action when a new message arrives
  for (int i=0; i<numNewMessages; i++) 
  {
    // Chat id of the requester It authenticates user via telegram ID first
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
    // Printing received message
    String user_text = bot.messages[i].text;
    Serial.println(user_text);
    String your_name = bot.messages[i].from_name;

    // What to do if user types /start -- this gives a gist of all available options
    if (user_text == "/start") 
    {
      String welcome = "Welcome, " + your_name + ".\n";   
      welcome += "We have following denominations.\n\n";
      welcome += "Rs. 2000 ; Rs. 1000 ; Rs. 500  \n\n";
      welcome += "Send /login to get otp to initiate transaction \n";    // to login to carry furthur transaction
      welcome += "Send /balance to get your available balance in account \n";   // to get balance 
      bot.sendMessage(chat_id, welcome, "");
    }

    // if balance option chosen
    if (user_text == "/balance"){
     int val_final ;
     val_final= get_balance();  // get outstanding amount from EEPROM
     Serial.println(val_final);
     
     String msg_bal = "Your Available Balance is Rs. " + String(BIG_BALANCE);
     bot.sendMessage(chat_id, msg_bal, "");
    }


    // If user gives login option
    if (user_text == "/login")
    {
      
      if (otp<10)  // in case otp is less hta 10 i.e single digit it pads a preceeding 0 before display
      {
        otp_final = '0'+String(otp);
      }
      else
      {
        otp_final = String(otp);
      }
      String msg1 = "Your OTP is : " + otp_final;
      bot.sendMessage(chat_id, msg1, "");
      // A delay of 4 secs + 6sec(inside read_value() function) to ensure sufficient time for user to select appropriate pin touch
      delay(4000);
      d1 = read_value();
      delay(4000);
      d2 = read_value();
      
      otp_typd = String(d1) + String(d2);
      Serial.println(otp_typd);
      msg1 = "You entered : " + otp_typd;
      bot.sendMessage(chat_id, msg1, "");

      // Authentication whether opt given is same as opt entered by used 
      if (otp_final.equals(otp_typd)){
        msg1 = "Authentication Approved \n Enter withdrawal amout via touch pins";
        bot.sendMessage(chat_id, msg1, "");
        delay(4000);
        r1 = read_value();
        delay(4000);
        r2 = read_value();
        delay(4000);
        r3 = read_value();
        delay(4000);
        r4 = read_value();

        //Displaying selected amount
        String amount = String(r1) + String(r2)+ String(r3)+ String(r4);
        msg1 = "Your entered amount : " + amount;
        bot.sendMessage(chat_id, msg1, "");
        int verdict = denomination_calcs(amount);  // calls denomination_calcs to check transaction process
 
        //handles cases of unavailability due to high amount, insufficient denominations and availability 
        if(verdict==0){ 
          bot.sendMessage(chat_id, "The value entered is higher than balance available. Try Again !!", "");
        }
        else if(verdict==1){
          bot.sendMessage(chat_id, "Withdrawal Successfull!! Please check balance using /balance or on ThingSpeak", "");  

          // printing balance and remaining denominations in serial monitor 
          int val_bal = get_balance();      
          BIG_BALANCE = BIG_BALANCE - amount.toInt();
          Serial.print("Amount Withdrawn : Rs. ");
          Serial.println(amount);
          Serial.print("Balance Amount in bank : Rs ");
          Serial.println(BIG_BALANCE); 

          Serial.print(denom2k);
          Serial.print(denom1k);
          Serial.println(denom500);

          //printing the same 4 parameters as above in ThingSpeak
          if(WiFi.status() == WL_CONNECTED)
          {
            HTTPClient http;
            http.begin(serverName);
            String DataSent = "api_key=" + myWriteAPIKey + "&field1=" + String(denom2k) + "&field2=" + String(denom1k)+ "&field3=" + String(denom500)+ "&field4=" + String(BIG_BALANCE);
            int Response = http.POST(DataSent);
            http.end();
            }
          
        }
        else if(verdict==2){
          bot.sendMessage(chat_id, "We do not have adequate denominations for the amount required !! Try Again!!", "");
        }
      }
      else{ // if otp does not match 
        msg1 = "Authentication failed. Please login again";
      }
    }
   }
   return otp;
}


int get_balance()
{
  // 2000 denomination notes
  int n1 = EEPROM.read(0);
  int val1 = n1*2000;
  //Serial.println(val1);

  // 1000 denomination notes
  int n2 = EEPROM.read(1);
  int val2 = n2*1000;
  //Serial.println(val2);

  // 500 denomination notes
  int n3 = EEPROM.read(2);
  int val3 = n3*500;
  //Serial.println(val3);

  // final outstanding
  int val_final = val1 + val2 + val3;
  return val_final;
}

int denomination_calcs(String amount)
{
  // get available balance and notes count 
  int balance = get_balance();
  int m = EEPROM.read(0);
  m = denom2k;
  int n = EEPROM.read(1);
  n = denom1k;
  int o = EEPROM.read(2);
  o = denom500;
  int amt = amount.toInt();
  if(amt>balance){  //in case amount desired is more than balance
    return 0;
  }
  else{  // Calculate if value can be given when amount is less than balance
    while (amt >= 2000 && m>0){
      amt = amt- 2000;
      //Serial.println(amt);
      m=m-1;
    }
    while (amt>=1000 && n>0){
      amt = amt-1000;
      //Serial.println(amt);
      n=n-1;
    }
    while (amt>=500 && o>0){
      amt = amt-500;
      //Serial.println(amt);
      o=o-1;
    }
    
    if(amt==0){  //if can be given update denominations
      EEPROM.write(0,m);
      EEPROM.write(1,n);
      EEPROM.write(2,o);
      EEPROM.commit();
      denom2k = m;
      denom1k = n;
      denom500=o;
      return 1;
    }
    else{ // if denominations insufficient
      return 2;
    }
  }  
}

int read_value()
{
  int digi1 = 1;  // Default digit value if not touched is 1
  Serial.println("We entered loop");
  delay(6000);
  // Which pins are configured to which digit can be found on report
    int T0 = touchRead(4);
    //int T2 =touchRead(2);
    int T3 = touchRead(15);
    int T4 = touchRead(13);
    int T5 = touchRead(12);
    int T6 = touchRead(14);
    int T7 = touchRead(27);
    int T8 = touchRead(33);
    int T9 = touchRead(32);
    int T2 = digitalRead(35);
    
    if (T0<40) {
      digi1 = 0;
      Serial.println("T0");
      Serial.print("digit 1: ");
    Serial.println(digi1);
    } 
    else if (T3<40) {
      digi1 = 3;
      Serial.println("T3");
      Serial.print("digit 1: ");
    Serial.println(digi1);
    } 
    else if (T4<40) {
      digi1 = 4;
      Serial.println("T4");
      Serial.print("digit 1: ");
    Serial.println(digi1);
    } 
    else if (T5<40) {
      digi1 = 5;
      Serial.println("T5");
      Serial.print("digit 1: ");
    Serial.println(digi1);
    } 
    else if (T6<40) {
      digi1 = 6;
      Serial.println("T6");
      Serial.print("digit 1: ");
    Serial.println(digi1);
    } 
    else if (T7<40) {
      digi1 = 7;
      Serial.println("T7");
      Serial.print("digit 1: ");
    Serial.println(digi1);
    } 
    else if (T8<40) {
      digi1 = 8;
      Serial.println("T8");
      Serial.print("digit 1: ");
    Serial.println(digi1);
    } 
    else if (T9<40) {
      digi1 = 9;
      Serial.println("T9");
      Serial.print("digit 1: ");
    Serial.println(digi1);
    } 
    else if (T2) {
      digi1 = 2;
      Serial.println("T2");
      Serial.print("digit 1: ");
    Serial.println(digi1);
    } 
    return digi1;
}

void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(0));
  //configuring pins
  pinMode(4, INPUT);
  pinMode(2, INPUT);
  pinMode(15, INPUT);
  pinMode(13, INPUT);
  pinMode(12, INPUT);
  pinMode(14, INPUT);
  pinMode(27, INPUT);
  pinMode(33, INPUT);
  pinMode(32, INPUT);
  pinMode(35,INPUT);

  //configuring EEPROM
  EEPROM.begin(EEPROM_SIZE);

  if (!EEPROM.begin(EEPROM_SIZE))
  {
    Serial.println("failed to initialise EEPROM"); 
  }
  EEPROM.put(0,5);
  EEPROM.put(1,10);
  EEPROM.put(2,10);
  Serial.println(EEPROM.read(1));
  Serial.println(EEPROM.read(2));

  
  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
}

void loop() {
  int otp_received= -1;
  if (millis() > lastTimeBotRan + bot_delay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("Got Response!");
      otp_received = handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
      if(otp_received != -1){
      Serial.println("OTP generated: ");
    }
    
    Serial.println("We are out of loop. !! LOGIN AGAIN NOW!!");
    lastTimeBotRan = millis();
  }
}
}
