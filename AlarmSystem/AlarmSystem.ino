/*Система, яка допоможе зберегти Ваше майно від крадіжки. Якщо Ваші двері відкриються, система сповістить Вас про це. 
Все працює на основі процесора ESP8266 на мікроконтроллері Node NCU V3. Можливість працювати з інтернетом дозволила
комунікувати з процесором за допомогою телеграм бота.
*/


// +++ НАЛАШТУВАННЯ ДЛЯ КОРИСТУВАЧА +++

//***Підключення біблотек***
#include <FastBot.h> // білбліотека для модерації ботом

//***Налаштування WiFi***
#define WIFI_SSID "********" //логін вашого Wi-Fi
#define WIFI_PASS "********" //пароль до вашого Wi-Fi

//***Налаштування роботи бота***
#define BOT_TOKEN "6284409299:AAFsilSQpiN9u2OqEyICzFpdHhaxooOL8JM" // Унікальний для кожного бота токен отриманий при створені
#define ChatID "*********" //унікальні ID для кожного чату бота з користувачем 
// Для додавання нових користувачів впишіть їх ID через коми без пробілу по типу: #define ChatID "000000000,111111111,..."
// Для отримання Вашого ID чату перейдіть на телеграм бот за посиланням: https://t.me/getidsbot    


#define pinClick 4

// +++ НАЛАШТУВАННЯ ДЛЯ РОЗРОБНИКА +++

//Розпіновка
#define buzzerPin D1    //пін зумера(пищалки)
#define gerconPin D2    //пін геркону
#define buttonPin D3    //пін кнопки
#define GreenledPin D7  //пін зеленого світлодіода
#define RedledPin D5    //пін червоного світлодіода

// Константи для таймерів
#define botTickTime 1000  //частота опитування бота
#define pinTickTime 100   //частота опитування пінів


FastBot bot(BOT_TOKEN);

// Таймери
unsigned long long pin_last_run = 0; //таймер для опитування пінів
unsigned long long bot_last_run = 0; //таймер для опитування бота

//Прапорці для роботи основного коду
bool is_duty_on = false;
bool ALARM = false;
bool button_flag = false;


//***Функції***

// Підключення до WiFi
void connectWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(333);
    digitalWrite(GreenledPin, !digitalRead(GreenledPin));
    if (millis() > 15000) ESP.restart();
  }
  digitalWrite(GreenledPin, 0);
}

// Апарате переривання для геркону
IRAM_ATTR void gercon() {
  if (is_duty_on) ALARM = true;
}

// Апарате переривання для кнопки
IRAM_ATTR void button() {
  ALARM = false;
  button_flag = true;
  is_duty_on = false;
  digitalWrite(buzzerPin,0);
  digitalWrite(RedledPin,0);
}

// Логіка бота
void newMsg(FB_msg& msg) {
  //Далі логіка кожної команди
  if (msg.text == "Увімкнути") {
    is_duty_on = true;
    digitalWrite(GreenledPin,1);
    bot.sendMessage("Сигналізація в режимі чергування", ChatID); 
  } 
  else if (msg.text == "Вимкнути") {
    button_flag = false;
    is_duty_on = false;
    ALARM = false; 
    digitalWrite(buzzerPin,0);
    digitalWrite(RedledPin,0);
    digitalWrite(GreenledPin,0);
    bot.sendMessage("Сигналізація вимкнена", ChatID); 
  } 
  else if (msg.text == "/start") {
    String welcome = "Вітаю, " + msg.username + "!\n\n";
    welcome += "Цей бот створено для віддаленого керування Вашою сигналізацією. \n\n";
    welcome += "    Щоб увімкнути сигналізацію натисніть кнопку 'Увімкнути' в меню знизу.";
    welcome += "Аналогічно для вимкнення.\n";
    welcome += "    Якщо хтось проникне до кімнати при увімкненній сигналізації - Ви побачите сповіщення.\n\n";
    welcome += "Бережіть себе та своє майно!)\n";

    bot.sendMessage(welcome, msg.chatID); //написати вітання
    bot.showMenu("Увімкнути \n Вимкнути"); //показати меню
  }
  else {
    bot.sendMessage("Вибачте, я не знаю такої команди", msg.chatID); 
  }
}

void setup() {
  // Налаштування розпіновки
  pinMode(gerconPin, INPUT);
  pinMode (buzzerPin, OUTPUT);
  pinMode (GreenledPin, OUTPUT);
  pinMode (RedledPin, OUTPUT);
  // Провсяк випадок
  digitalWrite(buzzerPin, 0); 
  //Налаштування бота
  bot.setChatID(ChatID);
  //Підключення WiFi
  connectWiFi();
  //Підключення боту
  bot.attach(newMsg);
  //Підключення апаратного переривання
  attachInterrupt(buttonPin, button, CHANGE );
  attachInterrupt(gerconPin, gercon, CHANGE );
  //Відправка в чат повідомлення про готовність до роботи
  bot.sendMessage("Сигналізація онлайн в режимі очікування", ChatID);
}


void loop() {
  if(  ALARM == false && button_flag == true && is_duty_on == false) { 
    bot.sendMessage("Сигналізацію вимкнуто, апаратно", ChatID);
    button_flag = false;
  }
  if ( millis() - bot_last_run > botTickTime) {
    bot.tickManual();
    bot_last_run = millis();
  }
  if (is_duty_on && millis() - pin_last_run > pinTickTime) {
    // digitalWrite(GreenledPin, 1);
    if (ALARM) {
      if (button_flag == false) {
        bot.sendMessage("!!!Тривога, хтось проник до Вашої кімнати!!!", ChatID);
        button_flag = true;
      }
      digitalWrite(buzzerPin, 1);
      digitalWrite(RedledPin,!digitalRead(RedledPin));
    }
    else {
      digitalWrite(buzzerPin, 0);
      digitalWrite(RedledPin, 0);
    }
    pin_last_run = millis();
  }
}


