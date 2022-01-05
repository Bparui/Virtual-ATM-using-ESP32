//Since in real ATM we can not just withdraw money or check balance but also deposit money that can be an addition that can be done to the code to make it more similar to ATM.\\

//The following code can be incorporated in appropriate place mentioned to have an additional feature of deposit in our telegram ATM.
\begin{lstlisting}{C/Cpp}
in that case under handleNewMessages():\\
//under if(user_text =/start) loop add this line
welcome += "Send /deposit to deposit some amount to your account";

//then create this additional 
if(user_text == "/deposit"){
    int dp1, dp2, dp3, dp4;
    delay(2000);
    dp1 = read_value();
    delay(2000);
    dp2 = read_value();
    delay(2000);
    dp3 = read_value();
    delay(2000);
    dp4 = read_value();
    
    String amount = String(dp1) + String(dp2)+ String(dp3)+ String(dp4);
    msg1 = "Your entered amount(for deposit) : " + amount;
    bot.sendMessage(chat_id, msg1, "");
    int verdict = denoms_deposit(amount);
    if(verdict==1){
        new_bal = get_balance();  
        bot.sendMessage(chat_id,"Amount was deposited \n New Balance: ", "");
        bot.sendMessage(chat_id,new_bal, "");
        }
    else{
    bot.sendMessage(chat_id,"Amount could not be deposited \n No appropriate division into 2000,1000,500 ", "");
    }  
}

//Come out of handleNewMessages//
//--------------------------------------------------//
//Create a new function 


int denoms_deposit(String amount){ //Checks if the desired deposit amount can be segregated using 2000, 1000 and 500 rupee notes.
    int m=0; //for counting 2000s
    int n = 0; // for counting 1000s
    int o = 0;  // for counting 500s
    int amt = amount.toInt();
    while (amt >= 2000 ){
      amt = amt- 2000;
      //Serial.println(amt);
      m=m+1;
    }
    while (amt >=1000 ){
      amt = amt-1000;
      //Serial.println(amt);
      n=n+1;
    }
    while (amt>=500 ){
      amt = amt-500;
      //Serial.println(amt);
      o=o+1;
    }
    if(amt==0){ //Yes the amout can be divided adequately into available denomination 
    new_m = EEPROM.read(0) + m; //available denominations increase
    new_n = EEPROM.read(1) + n;
    new_o = EEPROM.read(2) + o;
    EEPROM.write(0, new_m);
    EEPROM.write(1, new_n);
    EEPROM.write(2, new_o);
    return 1;
    }
    else{
    return 0; // denomination division not possible
    }
}
