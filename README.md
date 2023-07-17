# Сигналізація на базі ESP-12E та геркону

Магніт вмонтовується до двері, геркон у відповідному місці вмонтовується до дверної шибки. Геркон дає інформацію про стан дверей. При відчиненні дверей сигнал подається мікроконтролеру. Він в свою чергу розуміє, що двері були відчинені, після чого подається сигнал на зумер (пищалку) та через мережу повідомляє телеграм бота про відчинення дверей. 

В телеграм боті можна вказати режими активації сигналізації (Увімкнути/Вимкнути). При вказанні режиму "Вимкнути" пристрій ігнорує зміни показів датчика. В режимі "Увімкнути" сигналізація відстежує рух дверей.

#Живлення
Якщо живити схему припаявши джерело живлення до контактів VIN та GND - підійде джерело живлення 5-15 В постійного струму.
В іншому випадку можна живити схему через роз'єм microUSB на мікроконтроллері, використовуючи звичайний адаптер на 5 В.
