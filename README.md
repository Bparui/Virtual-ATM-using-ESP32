# Virtual ATM using ESP32

A virtual ATM system was created using ESP32 microprocessor.
* **Arduino Computing Platform** was used for coding and loading purposes. 
* **Telegram** was used for providing a user interface for protected access to ATM which followd OTP authentication before allowing access to ATM.
* **ThingSpeak** was used as a platform similar to online bank accounts on website where Available balance transaction history details could be accessed.

For furthur references and detailed understanding of working procedure refer the youtube video [here](https://www.youtube.com/watch?v=usMuDdFp1d4&feature=youtu.be)

### Input Method
The ESP32 has ten capacitive touch pins(GPIO). 

The pins:
We have a total of 9 GPIO touch pins , out of which 
\\GPIO 0 is not available on the board 
\\GPOI 2 does not take any inputs

So as a result the following connections were used \\
Digit 0  - GPIO4\\
Digit 1  -  Default value in case no pin is touched\\
Digit 2  - Connected to GPIO35(input pin) via hardware push button \\
Digit 3  - GPIO15\\
Digit 4  - GPIO13\\
Digit 5  - GPIO12\\
Digit 6  - GPIO14\\
Digit 7  - GPIO27\\
Digit 8  - GPIO33\\
Digit 9  - GPIO32\\

The connection of push button for Digit 2 is as follows(not the Led connections)

### Features
* Initially the ATM has Rs 25,000 and 10,10,5 notes of 500,100 and 2000 Rs denominations respectively. Any successful transaction leads to a change in these values (EEPROM) 
* Only a predefined Telegram used( in the code) can access the ATM Bot for transaction.
* A 2 digit OTP verification is done before acess to ATM account. The OTP generated has to be entered by user via Touch Pins of ESp32 in accordance with the predefined digits-pin set.
* The user can withdraw , deposit or view outstanding amount in the account via options in telegram. The amount to be transacted must be entered via the touch pins
* After each successful transaction the updated amount and denominations can be checked on IoT cloud server on ThingSpeak.

![](https://i.imgur.com/JVTawNC.png)
