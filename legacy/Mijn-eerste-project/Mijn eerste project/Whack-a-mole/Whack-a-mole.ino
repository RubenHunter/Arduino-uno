//Als eerste verklaren we de pinwaarden
int led1 = 2;
int led2 = 3;
int led3 = 4;

int drukknop1 = 9;
int drukknop2 = 8;
int drukknop3 = 7;

int beginknop = 6;

int vertraging = 250;

int score = 0;

void setup()
{
  //Seriële monitor starten.
  Serial.begin(9600);
  //De speler laten weten dat het programma is gestart.
  Serial.println("Welkom bij Whack-a-mole!!");
  //De speler uitleg over het spel geven.
  Serial.println("Uitleg: Druk op de knop rechts en houd het in totdat de lampjes stoppen met flikkeren");
  Serial.println("Druk dan op de knop onder het lampje dat aanspringt om een punt te verdienen");
  Serial.println("Verdien 20punten om te winnen");
  Serial.println("Veel succes!!");
  
  //Nu zetten we de leds als een OUTPUT
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  
  //Nu zetten we de drukknoppen als INPUT
  pinMode(drukknop1, INPUT_PULLUP);
  pinMode(drukknop2, INPUT_PULLUP);
  pinMode(drukknop3, INPUT_PULLUP);
  
  pinMode(beginknop, INPUT_PULLUP);
  
}

void loop()
{ 
  if(digitalRead(beginknop) == LOW){
  //Ik wil niet dat de game oneindig is maar eindigt bij 20keer stopt
    for (int h = 0; h < 20; h++){
    //Randeom led aan zetten
    digitalWrite(random(2, 5), HIGH);
    
    //checken of enige leds aan staan
    while (digitalRead(led1) == HIGH || digitalRead(led2) == HIGH || digitalRead(led3) == HIGH){
     
      //zien of het lampje aan is
      if (digitalRead(led1) == HIGH){
        //if (digitalRead(led1) == !LOW){   => vuile code
        //if (digitalRead(led1) != LOW){
        
        //zien of de knop is ingedrukt
        if (digitalRead(drukknop1)== LOW){
        //nadat de knop is ingedrukt het lampje uit zetten
        digitalWrite(led1, LOW);
      }
    }
    //nu hetzelfde doen voor de andere lampjes en drukknoppen
      if (digitalRead(led2) == HIGH){
        if (digitalRead(drukknop2) == LOW){
          digitalWrite(led2, LOW);
        }
      }
      if (digitalRead(led3) == HIGH){
        if (digitalRead(drukknop3) == LOW){
          digitalWrite(led3, LOW);
        }
      }
      //een vertraging zodat de lampjes niet te snel veranderen.
      delay(vertraging);
    }
    //de score bijhouden
    score++;
    Serial.println("Je score is:");
    Serial.println(score);
    }
  } 

//Om aan te tonen dat het spelletje gedaan is zullen de lampjes 5keer flikkeren
for (int g = 0; g < 5; g++){
  //lampjes aan flikkeren
  digitalWrite(led1, HIGH);
  digitalWrite(led2, HIGH);
  digitalWrite(led3, HIGH);
  delay(vertraging);
  
  //lampjes uit flikkeren
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);
  delay(vertraging);
 }
}
