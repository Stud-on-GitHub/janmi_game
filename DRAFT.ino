//I2C pins are on GPIO 5 and 4 for data and clock respectively

     
     /////-----Librairies-----/////
  
#include <Wire.h>   //Connection via I2C pour Arduino 1.6.5 ou antérieur 
#include "SSD1306.h"   //Librairie du controleur de l'écran OLED 0.96"
#include "pitches.h"   //Inclusion du fichier de notes
#include <NewPing.h>   //Librairie pour le capteur à ultrason HC-SR04 https://bitbucket.org/teckel12/arduino-new-ping/wiki/Home
#include <Adafruit_NeoPixel.h>   //Librairie pour les néopixels



     /////-----PARAMETRAGE DE L ECRAN-----/////
     
//Initialise l'écran OLED utilisant la librairie Wire:
SSD1306  display(0x3c, 5, 4);   //Différent pour lolin32!!!



     /////-----PARAMETRAGE DU BUZZER PIEZO-ELECTRIQUE-----/////
     
//Déclaration de la pin pour le buzzer:
int const piezoPin = 25;
//int const piezoPin = 1;

//Déclaration de la fréquence:
int freq;

//Déclaration du canal du PWM:
int const channel = 0;

//Déclaration de la résolution du nombre de bit spécifique au rapport cyclique:
int const resolution = 8;   //Rapport cyclique :  https://fr.wikipedia.org/wiki/Rapport_cyclique



     /////-----PARAMETRAGE DES CAPTEURS A ULTRASON-----/////
     
//Déclaration du nombre de capteur à l'ultrason:
int const sonarNb = 3;
//int const sonarNb = 4;

//Déclaration de la distance max (en cm) du capteur à l'ultrason:
int const distMax = 30;

//Déclaration des pin des capteurs à l'ultrason:
int const trigPinUltra1 = 14;   //Trigger pin 14
int const echoPinUltra1 = 15;   //Echo pin 15
int const trigPinUltra2 = 2;
int const echoPinUltra2 = 13;
int const trigPinUltra3 = 0;
int const echoPinUltra3 = 12;
//int const trigPinUltra4 = 25;
//int const echoPinUltra4 = 26;

//Déclaration du tableau du nouvel objet du capteur à l'ultrason:
NewPing sonar[sonarNb] = {
  NewPing(trigPinUltra1, echoPinUltra1, distMax),
  NewPing(trigPinUltra2, echoPinUltra2, distMax),
  NewPing(trigPinUltra3, echoPinUltra3, distMax), 
//  NewPing(trigPinUltra4, echoPinUltra4, distMax),  
};



     /////-----PARAMETRAGE DES NEOPIXELS-----/////

//Déclaration de la pin des néopixels:
int const pixelPin = 16;

//Déclaration du nombre de néopixel:
int const pixelSetNb = 3;
//int const pixelSetNb = 4;

//Déclaration de l'intensité lumineuse des néopixels:
int const intensiteLumMin = 5;

//Déclaration de l'intensité lumineuse max des néopixels:
int const intensiteLumMax = 20;

//Création et initilisation des objets luz:
Adafruit_NeoPixel luz = Adafruit_NeoPixel(pixelSetNb, pixelPin, NEO_GRB + NEO_KHZ800);



     /////-----DECLARATION DES VARIABLES GLOBALES-----/////

int jumRandNb[3];   //Tableau des chiffres générés au hasard  
//int jumRandNb[4];   //Tableau des chiffres générés au hasard
///int notePossible[] = {NOTE_C4, NOTE_E4, NOTE_G4};   //Tableau des notes possible en Hz(230, 330, 392 Hz)
//int notePossible[] = {NOTE_C4, NOTE_E4, NOTE_G4, NOTE_B4};   //Tableau des notes possible en Hz(230, 330, 392, 494 Hz)
int notePossible[] = {NOTE_C4, NOTE_E4, NOTE_G4, NOTE_D4, NOTE_F4, NOTE_A4};   //(230, 330, 392, 294, 349, 440 Hz)
int compteur = 0;   //stock le nombre de tour réussi
int genNoteTab[39];  //Tableau des notes générées, 39 étant l'âge du capitaine 
int noteJoueTab[39];   //Tableau des notes jouées, 39 étant l'âge du capitaine 
int nombreNoteJoue = 0;   //Stock le nombre de note joué 
int resultat = 0;   //Stock le résultat (0 = début ou non réussi, 1 = encore, 2 = tour réussi)
int vitesseNote = 600;   //Rapidité des notes jouées
int sonarTab[]= {39, 39};   //Tableau des sonars, 39 étant l'âge du capitaine
int resultat2 = 0;   //Stock le résultat N°2 (0 = pas de valeur, 1 = valeur obtenue)


     /////-----METHODE DE LA MELODIE DE REUSSITE-----/////
     
void melodieWin(){
  
  int melodie[] = {NOTE_C4, NOTE_C4, NOTE_G4, NOTE_C5, NOTE_G4, NOTE_C5};   //6 notes
  int duree[] = {100, 100, 100, 300, 100, 300};
  
  for (int thisNote = 0; thisNote < 6; thisNote ++) {
    
    ledcWriteTone(channel, melodie[thisNote]);   //Fonction du PWM qui définie la fréquence à jouer
    delay(duree[thisNote]);
    
   //Allume les néopixels:           
    luz.setPixelColor(0, luz.Color(255, 255, 150));     
    luz.setPixelColor(1, luz.Color(255, 255, 150));   
    luz.setPixelColor(2, luz.Color(255, 255, 150));  
//    luz.setPixelColor(3, luz.Color(255, 255, 150));    
   //Envoi la mise à jour de la couleur au hardware 
    luz.show(); 

    delay(90);
    
    ledcWriteTone(channel, 0);   //Et on coupe le son!

   //Eteint les néopixels:  
    luz.setPixelColor(0, luz.Color(0, 0, 0));  //Noir sur le premier néopixel -> 0
    luz.setPixelColor(1, luz.Color(0, 0, 0));
    luz.setPixelColor(2, luz.Color(0, 0, 0));
//    luz.setPixelColor(3, luz.Color(0, 0, 0));
   //Envoi la mise à jour de la couleur au hardware 
    luz.show(); 
    
  }
}



     /////-----METHODE DE LA MELODIE D ECHEC-----/////
     
void melodieLose(){
  
  int melodie[] = {NOTE_G3, NOTE_C3, NOTE_G3, NOTE_C3};   //4 notes
  int duree[] = {300, 300, 300, 300};
  
  for (int thisNote = 0; thisNote < 4; thisNote ++) {
    
    ledcWriteTone(channel, melodie[thisNote]);   //Fonction du PWM qui définie la fréquence à jouer
    delay(duree[thisNote]);

   //Allume les néopixels:
    luz.setPixelColor(0, luz.Color(255, 0, 0));   //Rouge sur le premier néopixel -> 0 
    luz.setPixelColor(1, luz.Color(255, 0, 0));
    luz.setPixelColor(2, luz.Color(255, 0, 0));
//    luz.setPixelColor(3, luz.Color(255, 0, 0));
   //Envoi la mise à jour de la couleur au hardware 
    luz.show(); 
    
    delay(90);
    
    ledcWriteTone(channel, 0);   //Et on coupe le son!

   //Eteint les néopixels:  
    luz.setPixelColor(0, luz.Color(0, 0, 0));  //Noir sur le premier néopixel -> 0
    luz.setPixelColor(1, luz.Color(0, 0, 0));
    luz.setPixelColor(2, luz.Color(0, 0, 0));
//    luz.setPixelColor(3, luz.Color(0, 0, 0));
   //Envoi la mise à jour de la couleur au hardware 
    luz.show(); 
    
  } 
}



     /////-----METHODE D AFFICHAGE DU DEBUT DU JANMI GAME-----/////
      
void affichageStart(){
 
 //Déclaration de la variable texte à afficher:
  String texte = "janmi game";

 //Définit le font initiale:
  display.setFont(ArialMT_Plain_10); 
 //Défini le texte au centre:
  display.setTextAlignment(TEXT_ALIGN_CENTER);
 //Positionne et défini le texte: 
  display.drawString(64, 25, texte); 
 //Affiche tout ce qu'il y a dans la mémoire d'affichage:
  display.display();
 //Donne du temps pour terminer la fonction : 
  delay(200);
 //Nettoie l'écran:
  display.clear(); 
 
 //Change la taille du Font en plus grand:
  display.setFont(ArialMT_Plain_16);
 //Défini le texte au centre:
  display.setTextAlignment(TEXT_ALIGN_CENTER); 
 //Positionne et défini le texte: 
  display.drawString(64, 22, texte);
 //Affiche tout ce qu'il y a dans la mémoire d'affichage:
  display.display();
 //Donne du temps pour terminer la fonction : 
  delay(200);
 //Nettoie l'écran:
  display.clear(); 
 
 //Change la taille du Font en plus grand et la typo:
  display.setFont(Mountains_of_Christmas_22);
 //Défini le texte au centre:
  display.setTextAlignment(TEXT_ALIGN_CENTER);
 //Positionne et défini la largeur max du texte: 
  display.drawStringMaxWidth(65, 15, 128, texte);  
 //Affiche tout ce qu'il y a dans la mémoire d'affichage:
  display.display();
 //Donne du temps pour terminer la fonction : 
  delay(200);
 //Nettoie l'écran:
  display.clear();

 //Change la taille du Font en plus grand et la typo:
  display.setFont(Mountains_of_Christmas_Bold_28);
 //Défini le texte au centre:
  display.setTextAlignment(TEXT_ALIGN_CENTER);
 //Positionne et défini la largeur max du texte: 
  display.drawStringMaxWidth(65, 10, 128, texte);  
 //Affiche tout ce qu'il y a dans la mémoire d'affichage:
  display.display();
 //Donne du temps pour terminer la fonction : 
  delay(5000);
 //Ecran noir: 
  display.clear();
  display.display();
  delay(50);   //Donne du temps pour terminer la fonction   

}



     /////-----METHODE D AFFICHAGE DU SCORE-----/////
      
void affichageScore(){
 
 //Déclaration de la variable texte à afficher:
  String texte = "SCORE :";
  String valeur = String (compteur);   // convertir l'entier compteur en chaine de character
 //Définit le font initiale:
  display.setFont(ArialMT_Plain_24); 
 //Défini le texte au centre:
  display.setTextAlignment(TEXT_ALIGN_CENTER);
 //Positionne et défini le texte: 
  display.drawString(50, 15, texte); 
 //Positionne et défini le texte: 
  display.drawString(115, 15, valeur);
 //Affiche tout ce qu'il y a dans la mémoire d'affichage:
  display.display();
 //Donne le temps d'affichage: 
  delay(4000);
 //Ecran noir: 
  display.clear();
  display.display();
  delay(50);   //Donne du temps pour terminer la fonction
  
}



     /////-----METHODE DEBUT DU JANMI GAME-----/////
      
void allStart(){  
  
  int melodie[] = {NOTE_C4, NOTE_F4, NOTE_C4, NOTE_F4, NOTE_C4, NOTE_F4, NOTE_C4, NOTE_F4, NOTE_G4, NOTE_F4, NOTE_E4, NOTE_F4, NOTE_G4};   //12 notes
  int duree[] = {100, 200, 100, 200, 100, 400, 100, 100, 100, 100, 200, 100, 500}; 

 //Joue une mélodie et allume progressivement les néopixels:
  for (int thisNote = 0; thisNote < 13; thisNote ++) {
    
   //Joue la note: 
    ledcWriteTone(channel, melodie[thisNote]);   //Fonction du PWM qui définie la fréquence à jouer
    delay(duree[thisNote]);
    ledcWriteTone(channel, 0);   //Et on coupe le son!
    
   //Définit l'intensité lumineuse 
    luz.setBrightness(thisNote);

   //Allume les néopixels en blanc:           
    luz.setPixelColor(0, luz.Color(255, 255, 150));     
    luz.setPixelColor(1, luz.Color(255, 255, 150));   
    luz.setPixelColor(2, luz.Color(255, 255, 150)); 
//    luz.setPixelColor(3, luz.Color(255, 255, 150));        
   //Envoi la mise à jour de la couleur au hardware 
    luz.show();   
     
    delay(100); 
    
  }
  
 //Allume en blanc et en accélérant les néopixels: 
  for (int i = 14; i < 30; i++){
    
   //Définit l'intensité lumineuse 
    luz.setBrightness(i);

   //Allume les néopixels:           
    luz.setPixelColor(0, luz.Color(255, 255, 150));     
    luz.setPixelColor(1, luz.Color(255, 255, 150));   
    luz.setPixelColor(2, luz.Color(255, 255, 150));  
//    luz.setPixelColor(3, luz.Color(255, 255, 150));     
   //Envoi la mise à jour de la couleur au hardware 
    luz.show(); 

   //diminue le temps de pause   
    if (i <= 19){
      delay(90); 
    }else if (i > 19 && i <= 24){ 
      delay(60); 
    }else if(i > 24){
      delay(30);
      
    }  
  } 
}



     /////-----METHODE DE JUMELAGE CAPTEURS A ULTRASON/NEOPIXEL ET NOTE-----/////
     
void jumelage(){
  
 //Génère au hasard 3 chiffres entre 0 et 2 qui sont tous différents: 
  jumRandNb[0] = random(0 , 3);
  for (int i = 1; i < 3; i++){
    jumRandNb[i] = random(0 , 3);
    for (int j = 0; j < i; j++){
      if (jumRandNb[j] == jumRandNb[i]){
        i = i -1;
        break;
      }
    } 
  }
/*
 //Génère au hasard 4 chiffres entre 0 et 3 qui sont tous différents: 
  jumRandNb[0] = random(0 , 4);
  for (int i = 1; i < 4; i++){
    jumRandNb[i] = random(0 , 4);
    for (int j = 0; j < i; j++){
      if (jumRandNb[j] == jumRandNb[i]){
        i = i -1;
        break;
      }
    } 
  }
*/  
}



     /////-----METHODE QUI GENERE LES NOTES-----/////
     
void genereNote(){
  
  delay (30);  
///  genNoteTab[compteur] = notePossible[random(0 , 3)];
//  genNoteTab[compteur] = notePossible[random(0 , 4)];
  genNoteTab[compteur] = notePossible[random(0 , 6)];
  
}



     /////-----METHODE DU BEUZEUL-----/////

void beuzeul(){
  
  delay(24);
  int beuzRand = random (0, 100);
Serial.print("beuzRand : ");
Serial.println(beuzRand);  
  delay(33);
  int beuzRandNb1 = random (0, 51);
  delay(37);
  int beuzRandNb2 = random (0, 51);
  int beuzResultat = 0;
  String valeur1 = String(beuzRandNb1);   // convertir l'entier beuzRandNb1 en chaine de character
  String valeur2 = String(beuzRandNb2);   // convertir l'entier beuzRandNb2 en chaine de character

  if (beuzRand% 4 == 0){ 
    
Serial.println("beuzRand % 7 == 0");      
   //Définit le font initiale:
    display.setFont(ArialMT_Plain_24); 
   //Défini le texte au centre:
    display.setTextAlignment(TEXT_ALIGN_CENTER);
   //Positionne et défini le texte: 
    display.drawString(25, 15, valeur1); 
   //Positionne et défini le texte: 
    display.drawString(65, 15, "+"); 
   //Positionne et défini le texte: 
    display.drawString(100, 15, valeur2);
   //Affiche tout ce qu'il y a dans la mémoire d'affichage:
    display.display();
   //Donne du temps pour terminer la fonction 
    delay(50);
    
   //Définit l'intensité Lumineuse à valeur max:
    luz.setBrightness(intensiteLumMax); 
   //Allume les néopixels:           
    luz.setPixelColor(0, luz.Color(255, 255, 150));     
    luz.setPixelColor(1, luz.Color(255, 255, 150));   
    luz.setPixelColor(2, luz.Color(255, 255, 150));  
//    luz.setPixelColor(3, luz.Color(255, 255, 150));    
   //Envoi la mise à jour de la couleur au hardware 
    luz.show(); 
  
    delay(3000);

   //Eteint les néopixels:  
    luz.setPixelColor(0, luz.Color(0, 0, 0));  //Noir sur le premier néopixel -> 0
    luz.setPixelColor(1, luz.Color(0, 0, 0));
    luz.setPixelColor(2, luz.Color(0, 0, 0));
//    luz.setPixelColor(3, luz.Color(0, 0, 0));
   //Envoi la mise à jour de la couleur au hardware 
    luz.show();  

   //Ecran noir: 
    display.clear();
    display.display();
    delay(50);   //Donne du temps pour terminer la fonction

   //Définit l'intensité Lumineuse à valeur min:
    luz.setBrightness(intensiteLumMin); 
   //Assombrie les néopixels:  
    luz.setPixelColor(0, luz.Color(0, 255, 0));  
//    luz.setPixelColor(1, luz.Color(0, 255, 0));
    luz.setPixelColor(2, luz.Color(255, 0, 0));
//    luz.setPixelColor(3, luz.Color(255, 0, 0)); 
   //Envoi la mise à jour de la couleur au hardware 
    luz.show();

    while (!beuzResultat){
      
      for (int i = 0; i < 3; i++){   //pour chaque sonar
//        for (int i = 0; i < 4; i++){   //pour chaque sonar  
      
        sonar[i].ping_cm();   //On mesure

        if (sonar[0].ping_cm() > 0 || sonar[1].ping_cm() > 0){
          
         //affecte la variable: 
          beuzResultat = 1;
          
         //Fonction du PWM qui définie la fréquence à jouer 
          ledcWriteTone(channel, NOTE_E4);   
          
         //Définit l'intensité Lumineuse à valeur max:
          luz.setBrightness(intensiteLumMax); 
         //Envoi la mise à jour de la couleur au hardware: 
          luz.show();
          
          delay(vitesseNote);
          
          ledcWriteTone(channel, 0);   //Et on coupe le son!
          
         //Eteint les néopixels:  
          luz.setPixelColor(0, luz.Color(0, 0, 0));  //Noir sur le premier néopixel correspondant
//          luz.setPixelColor(1, luz.Color(0, 0, 0));
          luz.setPixelColor(2, luz.Color(0, 0, 0)); 
//    luz.setPixelColor(3, luz.Color(0, 0, 0)); 
         //Envoi la mise à jour de la couleur au hardware 
          luz.show(); 
                 
          break;

        }else if (sonar[2].ping_cm() > 0){  
//        }else if (sonar[2].ping_cm() > 0 || sonar[3].ping_cm() > 0){

         //affecte la variable: 
          beuzResultat = 1; 
          
         //Eteint les néopixels:  
          luz.setPixelColor(0, luz.Color(0, 0, 0));  //Noir sur le premier néopixel correspondant
//          luz.setPixelColor(1, luz.Color(0, 0, 0));
          luz.setPixelColor(2, luz.Color(0, 0, 0)); 
//    luz.setPixelColor(3, luz.Color(0, 0, 0)); 
         //Envoi la mise à jour de la couleur au hardware 
          luz.show();
          
          melodieLose();
          
         //Affiche le score:
          affichageScore();
          
         //Affecte le résultat:
          resultat = 0;
          
         //Met à zéro le compteur:
          compteur = 0;
          
         //Efface le nombre des notes jouées: 
          nombreNoteJoue = 0;  
           
         //Efface le tableau de note généré et de note joué:
          for (int e = 0; e < 39; e++){
            genNoteTab[e] = 0;
            noteJoueTab[e] = 0; 
          }
          
          break;
          
        }else{         
          delay (30);         
        }
      }
    }
  } 
  delay(800);
}



     /////-----METHODE D INITIALISATION-----/////
     
void setup(){

 //Ouverture de la liaison série:
  Serial.begin(115200);
 //Saute une ligne sur le moniteur série:
  Serial.println(); 
  
 //Initialise l'UI et l'écan aussi:
  display.init();
 //Tourne l'écran:
  display.flipScreenVertically();
 //Défaut Font:
  display.setFont(ArialMT_Plain_10);   //Choix taille font : 10 16 24

 //Initialise le PWM pour le buzzer:
  ledcSetup(channel, freq, resolution);   //Configure le PWM pour le buzzer
  ledcAttachPin(piezoPin, channel);   //Attache le canal du PWM au GPIO

 //Démarre la communication avec les set de néopixel:
  luz.begin();
 //Définit l'intensité Lumineuse par défaut à valeur max:
  luz.setBrightness(intensiteLumMax); 
 //initialise tous les set de néopixel sur "off": 
  luz.show();
  luz.setPixelColor(0, luz.Color(0, 0, 0));  //Noir sur le premier néopixel -> 0
  luz.setPixelColor(1, luz.Color(0, 0, 0));
  luz.setPixelColor(2, luz.Color(0, 0, 0));
//  luz.setPixelColor(3, luz.Color(0, 0, 0));
  luz.show();
}



     /////-----PARAMETRAGE DE LA FONCTION LOOP-----/////
     
void loop(){ 

  if (resultat == 0){
  
    allStart();
  
    affichageStart();
  
   //Eteint les néopixels:  
    luz.setPixelColor(0, luz.Color(0, 0, 0));  //Noir sur le premier néopixel -> 0
    luz.setPixelColor(1, luz.Color(0, 0, 0));
    luz.setPixelColor(2, luz.Color(0, 0, 0)); 
//    luz.setPixelColor(3, luz.Color(0, 0, 0));
   //Envoi la mise à jour de la couleur au hardware: 
    luz.show(); 

    delay(2000);

    jumelage();

Serial.println("jumelage : ");
Serial.print("jumRandNb[0] -> ");
Serial.println(jumRandNb[0]);
Serial.print("jumRandNb[1] -> ");
Serial.println(jumRandNb[1]);
Serial.print("jumRandNb[2] -> ");
Serial.println(jumRandNb[2]);
//Serial.print("jumRandNb[3] -> ");
//Serial.println(jumRandNb[3]);
Serial.println();    

   //Allume en rouge le néopixel: 
    luz.setPixelColor(jumRandNb[0], luz.Color(255, 0, 0));
   //Allume en vert le néopixel:
    luz.setPixelColor(jumRandNb[1], luz.Color(0, 255, 0));
   //Allume en bleu le néopixel:
    luz.setPixelColor(jumRandNb[2], luz.Color(0, 0, 255)); 
   //Allume en jaune le néopixel:
//    luz.setPixelColor(jumRandNb[3], luz.Color(255, 255, 0)); 
   //Envoi la mise à jour de la couleur au hardware: 
    luz.show(); 

    delay(5000);

   //Eteint les néopixels:  
    luz.setPixelColor(jumRandNb[0], luz.Color(0, 0, 0));  //Noir sur le premier néopixel correspondant 
    luz.setPixelColor(jumRandNb[1], luz.Color(0, 0, 0));
    luz.setPixelColor(jumRandNb[2], luz.Color(0, 0, 0)); 
//    luz.setPixelColor(jumRandNb[3], luz.Color(0, 0, 0));
   //Envoi la mise à jour de la couleur au hardware: 
    luz.show();

    delay(800);

  } 

  if (resultat >= 0){
    
    resultat = 0;
///    beuzeul();
    genereNote();   //génère les notes

Serial.print("compteur : ");
Serial.println(compteur);
Serial.print("genNoteTab[compteur] : ");
Serial.println(genNoteTab[compteur]);
Serial.print("nombreNoteJoue : ");
Serial.println(nombreNoteJoue); 
Serial.println();  

   //joue les notes génèrées et allume les néopixels correspondants:
    for (int thisNote = 0; thisNote <= compteur; thisNote ++) {
    
      ledcWriteTone(channel, genNoteTab[thisNote]);   //Fonction du PWM qui définie la fréquence à jouer

     //Compare la note génèrée et la fait correspondre au néopixel:  
      switch (genNoteTab[thisNote]){ 
       
        case 262:      
         //Définit l'intensité Lumineuse à valeur max:
          luz.setBrightness(intensiteLumMax);
         //Allume en rouge le néopixel correspondant:
          luz.setPixelColor(jumRandNb[0], luz.Color(255, 0, 0));
         //Envoi la mise à jour de la couleur au hardware: 
          luz.show();
        break;
      
        case 330:
         //Définit l'intensité Lumineuse à valeur max:
          luz.setBrightness(intensiteLumMax);
         //Allume en vert le néopixel correspondant:
          luz.setPixelColor(jumRandNb[1], luz.Color(0, 255, 0));
         //Envoi la mise à jour de la couleur au hardware: 
          luz.show();        
        break;
      
        case 392:       
         //Définit l'intensité Lumineuse à valeur max:
          luz.setBrightness(intensiteLumMax);
         //Allume en bleu le néopixel correspondant:
          luz.setPixelColor(jumRandNb[2], luz.Color(0, 0, 255));
         //Envoi la mise à jour de la couleur au hardware:
          luz.show();     
        break;
/*
        case 494:       
         //Définit l'intensité Lumineuse à valeur max:
          luz.setBrightness(intensiteLumMax);
         //Allume en jaune le néopixel correspondant:
          luz.setPixelColor(jumRandNb[3], luz.Color(255, 255, 0));
         //Envoi la mise à jour de la couleur au hardware:
          luz.show();     
        break;
*/         
        case 294:     //(262, 330, 392, 294, 349, 440 Hz)  
         //Définit l'intensité Lumineuse à valeur max:
          luz.setBrightness(intensiteLumMax);
         //Allume en jaune le néopixel correspondant:
          luz.setPixelColor(jumRandNb[0], luz.Color(255, 255, 0));
         //Envoi la mise à jour de la couleur au hardware:
          luz.show();     
        break;

        case 349:       
         //Définit l'intensité Lumineuse à valeur max:
          luz.setBrightness(intensiteLumMax);
         //Allume en cyan le néopixel correspondant:
          luz.setPixelColor(jumRandNb[1], luz.Color(0, 255, 255));
         //Envoi la mise à jour de la couleur au hardware:
          luz.show();     
        break;

        case 440:       
         //Définit l'intensité Lumineuse à valeur max:
          luz.setBrightness(intensiteLumMax);
         //Allume en violet le néopixel correspondant:
          luz.setPixelColor(jumRandNb[2], luz.Color(255, 0, 255));
         //Envoi la mise à jour de la couleur au hardware:
          luz.show();     
        break;
      
      }
      
      delay(vitesseNote);
    
      ledcWriteTone(channel, 0);   //Et on coupe le son!

     //Eteint les néopixels:  
      luz.setPixelColor(jumRandNb[0], luz.Color(0, 0, 0));  //Noir sur le premier néopixel correspondant
      luz.setPixelColor(jumRandNb[1], luz.Color(0, 0, 0));
      luz.setPixelColor(jumRandNb[2], luz.Color(0, 0, 0)); 
//      luz.setPixelColor(jumRandNb[3], luz.Color(0, 0, 0)); 
     //Envoi la mise à jour de la couleur au hardware 
      luz.show(); 

      delay(800);

    }

    beuzeul();

   //Teste chaque note joué: 
    while (nombreNoteJoue <= compteur && genNoteTab[0] && resultat < 2) {   //Tand que l'utilisateur doit joué,et qu'il ne c'est pas trompé, ni réussi le tour

     //Définit l'intensité Lumineuse à valeur min:
      luz.setBrightness(intensiteLumMin); 
     //Assombrie les néopixels:  
      luz.setPixelColor(jumRandNb[0], luz.Color(255, 0, 0));  
      luz.setPixelColor(jumRandNb[1], luz.Color(0, 255, 0));
      luz.setPixelColor(jumRandNb[2], luz.Color(0, 0, 255));
//      luz.setPixelColor(jumRandNb[3], luz.Color(255, 255, 0)); 
     //Envoi la mise à jour de la couleur au hardware 
      luz.show();
    
      for (int i = 0; i < 3; i++){   //pour chaque sonar
//      for (int i = 0; i < 4; i++){   //pour chaque sonar  
      
        sonar[jumRandNb[i]].ping_cm();   //On mesure 

///        if (sonar[jumRandNb[i]].ping_cm() > 0){   //S'il y a détection du sonar    
        if (sonar[jumRandNb[i]].ping_cm() > 0 && (genNoteTab[nombreNoteJoue] != 294 && genNoteTab[nombreNoteJoue] != 349 && genNoteTab[nombreNoteJoue] != 440)){   //S'il y a détection du sonar qui ne correspond pas aux couleurs secondaire  
             
          if (jumRandNb[i] == jumRandNb[0]){   //Vérifie quel est le sonar/couleur/note activé     
                   
            noteJoueTab[nombreNoteJoue] = 262;                
Serial.print("nombreNoteJoue : ");
Serial.println(nombreNoteJoue);
Serial.print("noteJoueTab[nombreNoteJoue] : ");
Serial.println(noteJoueTab[nombreNoteJoue]);
Serial.println();      
            ledcWriteTone(channel, noteJoueTab[nombreNoteJoue]);   //Fonction du PWM qui définie la fréquence à jouer
           //Définit l'intensité Lumineuse à valeur max:
            luz.setBrightness(intensiteLumMax); 
           //Allume en rouge le néopixel correspondant:
            luz.setPixelColor(jumRandNb[i], luz.Color(255, 0, 0));
           //Envoi la mise à jour de la couleur au hardware: 
            luz.show();
            delay(vitesseNote);
            ledcWriteTone(channel, 0);   //Et on coupe le son!
           //Définit l'intensité Lumineuse à valeur min:
            luz.setBrightness(intensiteLumMin); 
           //Assombrie les néopixels:  
            luz.setPixelColor(jumRandNb[i], luz.Color(255, 0, 0));   
           //Envoi la mise à jour de la couleur au hardware 
            luz.show();
            delay(600); 
           
            if (noteJoueTab[nombreNoteJoue] != genNoteTab[nombreNoteJoue]){   //Si la note joué est fausse      
               
              melodieLose();
              
             //Affiche le score:
              affichageScore();

             //Affecte le résultat:
              resultat = 0;

             //Met à zéro le compteur:
              compteur = 0;

             //Efface le nombre des notes jouées: 
              nombreNoteJoue = 0;   

             //Efface le tableau de note généré et de note joué:
              for (int e = 0; e < 39; e++){
                genNoteTab[e] = 0;
                noteJoueTab[e] = 0; 
              }
Serial.println("lose");
Serial.print("compteur: ");
Serial.println(compteur);
Serial.print("resultat: ");
Serial.println(resultat);
Serial.print("nombreNoteJoue: ");
Serial.println(nombreNoteJoue);
Serial.print("genNoteTab[0]: ");
Serial.println(genNoteTab[0]);
Serial.println();
                 
            }else if (nombreNoteJoue == compteur){   //Teste la correspondance de la dernière note de la série jouée et la dernière note générée
    
              melodieWin(); 
    
              delay(800);

             //Efface le tableau de note joué:
              for (int e = 0; e < 39; e++){
                noteJoueTab[e] = 0; 
              } 
        
             //incrémente de +1 le nombre de tour:
              compteur ++; 

             //Efface le nombre des notes jouées: 
              nombreNoteJoue = 0;

             //incrémente de +1 le résultat: 
              resultat = 2;
Serial.println("win");
Serial.print("compteur: ");
Serial.println(compteur);
Serial.print("resultat: ");
Serial.println(resultat);
Serial.print("nombreNoteJoue: ");
Serial.println(nombreNoteJoue);
Serial.println();

            }else { 
                              
              resultat = 1;
                
              nombreNoteJoue ++;
Serial.println("encore");
Serial.print("resultat: ");
Serial.println(resultat);
Serial.print("nombreNoteJoue: ");
Serial.println(nombreNoteJoue);
Serial.println();                
            }
              
          }else if (jumRandNb[i] == jumRandNb[1]){  //Vérifie quel est le sonar/couleur/note activé 
            
            noteJoueTab[nombreNoteJoue] = 330;
Serial.print("nombreNoteJoue : ");
Serial.println(nombreNoteJoue);
Serial.print("noteJoueTab[nombreNoteJoue] : ");
Serial.println(noteJoueTab[nombreNoteJoue]);
Serial.println();
            ledcWriteTone(channel, noteJoueTab[nombreNoteJoue]);   //Fonction du PWM qui définie la fréquence à jouer
           //Définit l'intensité Lumineuse à valeur max:
            luz.setBrightness(intensiteLumMax); 
           //Allume en vert le néopixel correspondant:
            luz.setPixelColor(jumRandNb[i], luz.Color(0, 255, 0));
           //Envoi la mise à jour de la couleur au hardware: 
            luz.show();
            delay(vitesseNote);
            ledcWriteTone(channel, 0);   //Et on coupe le son!
           //Définit l'intensité Lumineuse à valeur min:
            luz.setBrightness(intensiteLumMin); 
           //Assombrie le néopixel:   
            luz.setPixelColor(jumRandNb[i], luz.Color(0, 255, 0)); 
           //Envoi la mise à jour de la couleur au hardware 
            luz.show();
            delay(600);
            
            if (noteJoueTab[nombreNoteJoue] != genNoteTab[nombreNoteJoue]){   //Si la note joué est fausse   
                        
              melodieLose();
             
             //Affiche le score:
              affichageScore();

             //Affecte le résultat:
              resultat = 0;

             //Met à zéro le compteur:
              compteur = 0; 

             //Efface le nombre des notes jouées: 
              nombreNoteJoue = 0;  

             //Efface le tableau de note généré et de note joué:
              for (int e = 0; e < 39; e++){
                genNoteTab[e] = 0;
                noteJoueTab[e] = 0; 
              }
Serial.println("lose");
Serial.print("compteur: ");
Serial.println(compteur);
Serial.print("resultat: ");
Serial.println(resultat);
Serial.print("nombreNoteJoue: ");
Serial.println(nombreNoteJoue);
Serial.print("genNoteTab[0]: ");
Serial.println(genNoteTab[0]);
Serial.println();
 
            }else if (nombreNoteJoue == compteur){   //Teste la correspondance de la dernière note de la série jouée et la dernière note générée
    
              melodieWin(); 
    
              delay(800);

             //Efface le tableau de note joué:
              for (int e = 0; e < 39; e++){
                noteJoueTab[e] = 0; 
              } 
        
             //incrémente de +1 le nombre de tour:
              compteur ++; 

             //Efface le nombre des notes jouées: 
              nombreNoteJoue = 0;

             //incrémente de +1 le résultat: 
              resultat = 2; 
Serial.println("win");
Serial.print("compteur: ");
Serial.println(compteur);
Serial.print("resultat: ");
Serial.println(resultat);
Serial.print("nombreNoteJoue: ");
Serial.println(nombreNoteJoue);
Serial.println();

            }else {  
                              
              resultat = 1;
                
              nombreNoteJoue ++;
Serial.println("encore");
Serial.print("resultat: ");
Serial.println(resultat);
Serial.print("nombreNoteJoue: ");
Serial.println(nombreNoteJoue);
Serial.println();                
            }
              
          }else if (jumRandNb[i] == jumRandNb[2]){   //Vérifie quel est le sonar/couleur/note activé
            
            noteJoueTab[nombreNoteJoue] = 392;
Serial.print("nombreNoteJoue : ");
Serial.println(nombreNoteJoue);
Serial.print("noteJoueTab[nombreNoteJoue] : ");
Serial.println(noteJoueTab[nombreNoteJoue]);
Serial.println();        
            ledcWriteTone(channel, noteJoueTab[nombreNoteJoue]);   //Fonction du PWM qui définie la fréquence à jouer
           //Définit l'intensité Lumineuse à valeur max:
            luz.setBrightness(intensiteLumMax); 
           //Allume en bleu le néopixel correspondant:
            luz.setPixelColor(jumRandNb[i], luz.Color(0, 0, 255));
           //Envoi la mise à jour de la couleur au hardware: 
            luz.show();
            delay(vitesseNote);
            ledcWriteTone(channel, 0);   //Et on coupe le son!
           //Définit l'intensité Lumineuse à valeur min:
            luz.setBrightness(intensiteLumMin); 
           //Assombrie le néopixel:   
            luz.setPixelColor(jumRandNb[i], luz.Color(0, 0, 255)); 
           //Envoi la mise à jour de la couleur au hardware 
            luz.show();
            delay(600); 
            
            if (noteJoueTab[nombreNoteJoue] != genNoteTab[nombreNoteJoue]){   //Si la note joué est fausse  
       
              melodieLose();
               
             //Affiche le score:
              affichageScore();

             //Affecte le résultat:
              resultat = 0;

             //Met à zéro le compteur:
              compteur = 0;

             //Efface le nombre des notes jouées: 
              nombreNoteJoue = 0;  

             //Efface le tableau de note généré et de note joué:
              for (int e = 0; e < 39; e++){
                genNoteTab[e] = 0;
                noteJoueTab[e] = 0; 
              }
Serial.println("lose");
Serial.print("compteur: ");
Serial.println(compteur);
Serial.print("resultat: ");
Serial.println(resultat);
Serial.print("nombreNoteJoue: ");
Serial.println(nombreNoteJoue);
Serial.print("genNoteTab[0]: ");
Serial.println(genNoteTab[0]);
Serial.println();
                
            }else if (nombreNoteJoue == compteur){   //Teste la correspondance de la dernière note de la série jouée et la dernière note générée
    
              melodieWin(); 
    
              delay(800);

             //Efface le tableau de note joué:
              for (int e = 0; e < 39; e++){
                noteJoueTab[e] = 0; 
              } 
       
             //incrémente de +1 le nombre de tour:
              compteur ++; 

             //Efface le nombre des notes jouées: 
              nombreNoteJoue = 0;

             //incrémente de +1 le résultat: 
              resultat = 2; 
Serial.println("win");
Serial.print("compteur: ");
Serial.println(compteur);
Serial.print("resultat: ");
Serial.println(resultat);
Serial.print("nombreNoteJoue: ");
Serial.println(nombreNoteJoue);
Serial.println();

            }else {  
                              
              resultat = 1;
                
              nombreNoteJoue ++;
Serial.println("encore");
Serial.print("resultat: ");
Serial.println(resultat);
Serial.print("nombreNoteJoue: ");
Serial.println(nombreNoteJoue);
Serial.println();              
            }  
          }/*else if (jumRandNb[i] == jumRandNb[3]){   //Vérifie quel est le sonar/couleur/note activé
            
            noteJoueTab[nombreNoteJoue] = 494;
Serial.print("nombreNoteJoue : ");
Serial.println(nombreNoteJoue);
Serial.print("noteJoueTab[nombreNoteJoue] : ");
Serial.println(noteJoueTab[nombreNoteJoue]);
Serial.println();        
            ledcWriteTone(channel, noteJoueTab[nombreNoteJoue]);   //Fonction du PWM qui définie la fréquence à jouer
           //Définit l'intensité Lumineuse à valeur max:
            luz.setBrightness(intensiteLumMax); 
           //Allume en jaune le néopixel correspondant:
            luz.setPixelColor(jumRandNb[i], luz.Color(255, 255, 0));
           //Envoi la mise à jour de la couleur au hardware: 
            luz.show();
            delay(vitesseNote);
            ledcWriteTone(channel, 0);   //Et on coupe le son!
           //Définit l'intensité Lumineuse à valeur min:
            luz.setBrightness(intensiteLumMin); 
           //Assombrie le néopixel:   
            luz.setPixelColor(jumRandNb[i], luz.Color(255, 255, 0)); 
           //Envoi la mise à jour de la couleur au hardware 
            luz.show();
            delay(600); 
            
            if (noteJoueTab[nombreNoteJoue] != genNoteTab[nombreNoteJoue]){   //Si la note joué est fausse  
       
              melodieLose();
               
             //Affiche le score:
              affichageScore();

             //Affecte le résultat:
              resultat = 0;

             //Met à zéro le compteur:
              compteur = 0;

             //Efface le nombre des notes jouées: 
              nombreNoteJoue = 0;  

             //Efface le tableau de note généré et de note joué:
              for (int e = 0; e < 39; e++){
                genNoteTab[e] = 0;
                noteJoueTab[e] = 0; 
              }
Serial.println("lose");
Serial.print("compteur: ");
Serial.println(compteur);
Serial.print("resultat: ");
Serial.println(resultat);
Serial.print("nombreNoteJoue: ");
Serial.println(nombreNoteJoue);
Serial.print("genNoteTab[0]: ");
Serial.println(genNoteTab[0]);
Serial.println();
                
            }else if (nombreNoteJoue == compteur){   //Teste la correspondance de la dernière note de la série jouée et la dernière note générée
    
              melodieWin(); 
    
              delay(800);

             //Efface le tableau de note joué:
              for (int e = 0; e < 39; e++){
                noteJoueTab[e] = 0; 
              } 
       
             //incrémente de +1 le nombre de tour:
              compteur ++; 

             //Efface le nombre des notes jouées: 
              nombreNoteJoue = 0;

             //incrémente de +1 le résultat: 
              resultat = 2; 
Serial.println("win");
Serial.print("compteur: ");
Serial.println(compteur);
Serial.print("resultat: ");
Serial.println(resultat);
Serial.print("nombreNoteJoue: ");
Serial.println(nombreNoteJoue);
Serial.println();

            }else {  
                              
              resultat = 1;
                
              nombreNoteJoue ++;
Serial.println("encore");
Serial.print("resultat: ");
Serial.println(resultat);
Serial.print("nombreNoteJoue: ");
Serial.println(nombreNoteJoue);
Serial.println();              
            }  
          }
*/          
Serial.println("break 1");
Serial.println(); 
          break;   //Permet de sortir de la boucle qui teste chaque sonar
              
        }else if (sonar[jumRandNb[i]].ping_cm() > 0 && (genNoteTab[nombreNoteJoue] == 294 || genNoteTab[nombreNoteJoue] == 349 || genNoteTab[nombreNoteJoue] == 440)){
Serial.print("note secondaire attendu !");
Serial.println();          
          sonarTab[0] = jumRandNb[i];
         
          if (sonarTab[1] == 39){
Serial.print("sonarTab[1] == 39");
Serial.println();            
            if (i == 0){
Serial.print("i == 0");
Serial.println();             
              while (!resultat2) {

                delay(30);
                sonar[jumRandNb[1]].ping_cm();   //On mesure
                
                if(sonar[jumRandNb[1]].ping_cm()){                 
                  sonarTab[1]= jumRandNb[1];
                  resultat2 = 1;
                  break;                 
                }
                
                delay(30);
                sonar[jumRandNb[2]].ping_cm();   //On mesure

                if(sonar[jumRandNb[2]].ping_cm()){                 
                  sonarTab[1]= jumRandNb[2];
                  resultat2 = 1;
                  break;                 
                }                 
              }
              resultat2 = 0;   //Réinitialise la valeur du résultat N°2               
            }
            if (i == 1){
Serial.print("i == 1");
Serial.println();              
              while (!resultat2) {

                delay(30);
                sonar[jumRandNb[0]].ping_cm();   //On mesure
                
                if(sonar[jumRandNb[0]].ping_cm()){                 
                  sonarTab[1]= jumRandNb[0];
                  resultat2 = 1;
                  break;                 
                }
                
                delay(30);
                sonar[jumRandNb[2]].ping_cm();   //On mesure

                if(sonar[jumRandNb[2]].ping_cm()){                 
                  sonarTab[1]= jumRandNb[2];
                  resultat2 = 1;
                  break;                 
                }                 
              }
              resultat2 = 0;   //Réinitialise la valeur du résultat N°2               
            }
            if (i == 2){
Serial.print("i == 2");
Serial.println();              
              while (!resultat2) {

                delay(30);
                sonar[jumRandNb[1]].ping_cm();   //On mesure
                
                if(sonar[jumRandNb[1]].ping_cm()){                 
                  sonarTab[1]= jumRandNb[1];
                  resultat2 = 1;
                  break;                 
                }
                
                delay(30);
                sonar[jumRandNb[0]].ping_cm();   //On mesure

                if(sonar[jumRandNb[0]].ping_cm()){                 
                  sonarTab[1]= jumRandNb[0];
                  resultat2 = 1;
                  break;                 
                }                 
              } 
              resultat2 = 0;   //Réinitialise la valeur du résultat N°2            
            }            
          }
          
          if ((sonarTab[0] == jumRandNb[0] && sonarTab[1] == jumRandNb[1]) || (sonarTab[0] == jumRandNb[1] && sonarTab[1] == jumRandNb[0])){
            
           //294->jaune ? 
Serial.print("jaune");
Serial.println();
            noteJoueTab[nombreNoteJoue] = 294;
Serial.print("nombreNoteJoue : ");
Serial.println(nombreNoteJoue);
Serial.print("noteJoueTab[nombreNoteJoue] : ");
Serial.println(noteJoueTab[nombreNoteJoue]);
Serial.println();        
            ledcWriteTone(channel, noteJoueTab[nombreNoteJoue]);   //Fonction du PWM qui définie la fréquence à jouer
           //Définit l'intensité Lumineuse à valeur max:
            luz.setBrightness(intensiteLumMax); 
           //Allume en jaune le néopixel correspondant:
            luz.setPixelColor(jumRandNb[0], luz.Color(255, 255, 0));
            luz.setPixelColor(jumRandNb[1], luz.Color(255, 255, 0));
           //Envoi la mise à jour de la couleur au hardware: 
            luz.show();
            delay(vitesseNote);
            ledcWriteTone(channel, 0);   //Et on coupe le son!
           //Définit l'intensité Lumineuse à valeur min:
            luz.setBrightness(intensiteLumMin); 
           //Assombrie le néopixel:   
            luz.setPixelColor(jumRandNb[0], luz.Color(255, 0, 0));
            luz.setPixelColor(jumRandNb[1], luz.Color(0, 255, 0)); 
           //Envoi la mise à jour de la couleur au hardware 
            luz.show();
            delay(600); 
            
            if (noteJoueTab[nombreNoteJoue] != genNoteTab[nombreNoteJoue]){   //Si la note joué est fausse  
       
              melodieLose();
               
             //Affiche le score:
              affichageScore();

             //Affecte le résultat:
              resultat = 0;

             //Met à zéro le compteur:
              compteur = 0;

             //Efface le nombre des notes jouées: 
              nombreNoteJoue = 0;  

             //Efface le tableau de note généré et de note joué:
              for (int e = 0; e < 39; e++){
                genNoteTab[e] = 0;
                noteJoueTab[e] = 0; 
              }

             //Met à zéro le tableau des sonars: 
              sonarTab[0]= 39;
              sonarTab[1]= 39;

Serial.println("lose");
Serial.print("compteur: ");
Serial.println(compteur);
Serial.print("resultat: ");
Serial.println(resultat);
Serial.print("nombreNoteJoue: ");
Serial.println(nombreNoteJoue);
Serial.print("genNoteTab[0]: ");
Serial.println(genNoteTab[0]);
Serial.println();
                
            }else if (nombreNoteJoue == compteur){   //Teste la correspondance de la dernière note de la série jouée et la dernière note générée
    
              melodieWin(); 
    
              delay(800);

             //Efface le tableau de note joué:
              for (int e = 0; e < 39; e++){
                noteJoueTab[e] = 0; 
              } 
       
             //incrémente de +1 le nombre de tour:
              compteur ++; 

             //Met à zéro le nombre des notes jouées: 
              nombreNoteJoue = 0;

             //incrémente de +1 le résultat: 
              resultat = 2; 

             //Met à zéro le tableau des sonars: 
              sonarTab[0]= 39;
              sonarTab[1]= 39;

Serial.println("win");
Serial.print("compteur: ");
Serial.println(compteur);
Serial.print("resultat: ");
Serial.println(resultat);
Serial.print("nombreNoteJoue: ");
Serial.println(nombreNoteJoue);
Serial.println();

            }else {  
                              
              resultat = 1;

              nombreNoteJoue ++;

             //Met à zéro le tableau des sonars: 
              sonarTab[0]= 39;
              sonarTab[1]= 39;
           
Serial.println("encore");
Serial.print("resultat: ");
Serial.println(resultat);
Serial.print("nombreNoteJoue: ");
Serial.println(nombreNoteJoue);
Serial.println();              
            }          
          }else if ((sonarTab[0] == jumRandNb[1] && sonarTab[1] == jumRandNb[2]) || (sonarTab[0] == jumRandNb[2] && sonarTab[1] == jumRandNb[1])){
            
            //349->cyan ?
Serial.print("cyan");
Serial.println();
            noteJoueTab[nombreNoteJoue] = 349;
Serial.print("nombreNoteJoue : ");
Serial.println(nombreNoteJoue);
Serial.print("noteJoueTab[nombreNoteJoue] : ");
Serial.println(noteJoueTab[nombreNoteJoue]);
Serial.println();        
            ledcWriteTone(channel, noteJoueTab[nombreNoteJoue]);   //Fonction du PWM qui définie la fréquence à jouer
           //Définit l'intensité Lumineuse à valeur max:
            luz.setBrightness(intensiteLumMax); 
           //Allume en jaune le néopixel correspondant:
            luz.setPixelColor(jumRandNb[1], luz.Color(0, 255, 255));
            luz.setPixelColor(jumRandNb[2], luz.Color(0, 255, 255));
           //Envoi la mise à jour de la couleur au hardware: 
            luz.show();
            delay(vitesseNote);
            ledcWriteTone(channel, 0);   //Et on coupe le son!
           //Définit l'intensité Lumineuse à valeur min:
            luz.setBrightness(intensiteLumMin); 
           //Assombrie le néopixel:   
            luz.setPixelColor(jumRandNb[1], luz.Color(0, 255, 0));
            luz.setPixelColor(jumRandNb[2], luz.Color(0, 0, 255)); 
           //Envoi la mise à jour de la couleur au hardware 
            luz.show();
            delay(600); 
            
            if (noteJoueTab[nombreNoteJoue] != genNoteTab[nombreNoteJoue]){   //Si la note joué est fausse  
       
              melodieLose();
               
             //Affiche le score:
              affichageScore();

             //Affecte le résultat:
              resultat = 0;

             //Met à zéro le compteur:
              compteur = 0;

             //Efface le nombre des notes jouées: 
              nombreNoteJoue = 0;  

             //Efface le tableau de note généré et de note joué:
              for (int e = 0; e < 39; e++){
                genNoteTab[e] = 0;
                noteJoueTab[e] = 0; 
              }

             //Met à zéro le tableau des sonars: 
              sonarTab[0]= 39;
              sonarTab[1]= 39;

Serial.println("lose");
Serial.print("compteur: ");
Serial.println(compteur);
Serial.print("resultat: ");
Serial.println(resultat);
Serial.print("nombreNoteJoue: ");
Serial.println(nombreNoteJoue);
Serial.print("genNoteTab[0]: ");
Serial.println(genNoteTab[0]);
Serial.println();
                
            }else if (nombreNoteJoue == compteur){   //Teste la correspondance de la dernière note de la série jouée et la dernière note générée
    
              melodieWin(); 
    
              delay(800);

             //Efface le tableau de note joué:
              for (int e = 0; e < 39; e++){
                noteJoueTab[e] = 0; 
              } 
       
             //incrémente de +1 le nombre de tour:
              compteur ++; 

             //Met à zéro le nombre des notes jouées: 
              nombreNoteJoue = 0;

             //incrémente de +1 le résultat: 
              resultat = 2; 

             //Met à zéro le tableau des sonars: 
              sonarTab[0]= 39;
              sonarTab[1]= 39;

Serial.println("win");
Serial.print("compteur: ");
Serial.println(compteur);
Serial.print("resultat: ");
Serial.println(resultat);
Serial.print("nombreNoteJoue: ");
Serial.println(nombreNoteJoue);
Serial.println();

            }else {  
                              
              resultat = 1;

              nombreNoteJoue ++;

             //Met à zéro le tableau des sonars: 
              sonarTab[0]= 39;
              sonarTab[1]= 39;
         
Serial.println("encore");
Serial.print("resultat: ");
Serial.println(resultat);
Serial.print("nombreNoteJoue: ");
Serial.println(nombreNoteJoue);
Serial.println();              
            } 
          }else if ((sonarTab[0] == jumRandNb[2] && sonarTab[1] == jumRandNb[0]) || (sonarTab[0] == jumRandNb[0] && sonarTab[1] == jumRandNb[2])){
            
            //440->violet ?  
Serial.print("violet");
Serial.println(); 
            noteJoueTab[nombreNoteJoue] = 440;
Serial.print("nombreNoteJoue : ");
Serial.println(nombreNoteJoue);
Serial.print("noteJoueTab[nombreNoteJoue] : ");
Serial.println(noteJoueTab[nombreNoteJoue]);
Serial.println();        
            ledcWriteTone(channel, noteJoueTab[nombreNoteJoue]);   //Fonction du PWM qui définie la fréquence à jouer
           //Définit l'intensité Lumineuse à valeur max:
            luz.setBrightness(intensiteLumMax); 
           //Allume en jaune le néopixel correspondant:
            luz.setPixelColor(jumRandNb[2], luz.Color(255, 0, 255));
            luz.setPixelColor(jumRandNb[0], luz.Color(255, 0, 255));
           //Envoi la mise à jour de la couleur au hardware: 
            luz.show();
            delay(vitesseNote);
            ledcWriteTone(channel, 0);   //Et on coupe le son!
           //Définit l'intensité Lumineuse à valeur min:
            luz.setBrightness(intensiteLumMin); 
           //Assombrie le néopixel:   
            luz.setPixelColor(jumRandNb[2], luz.Color(0, 0, 255));
            luz.setPixelColor(jumRandNb[0], luz.Color(255, 0, 0)); 
           //Envoi la mise à jour de la couleur au hardware 
            luz.show();
            delay(600); 
            
            if (noteJoueTab[nombreNoteJoue] != genNoteTab[nombreNoteJoue]){   //Si la note joué est fausse  
       
              melodieLose();
               
             //Affiche le score:
              affichageScore();

             //Affecte le résultat:
              resultat = 0;

             //Met à zéro le compteur:
              compteur = 0;

             //Efface le nombre des notes jouées: 
              nombreNoteJoue = 0;  

             //Efface le tableau de note généré et de note joué:
              for (int e = 0; e < 39; e++){
                genNoteTab[e] = 0;
                noteJoueTab[e] = 0; 
              }

             //Met à zéro le tableau des sonars: 
              sonarTab[0]= 39;
              sonarTab[1]= 39;

Serial.println("lose");
Serial.print("compteur: ");
Serial.println(compteur);
Serial.print("resultat: ");
Serial.println(resultat);
Serial.print("nombreNoteJoue: ");
Serial.println(nombreNoteJoue);
Serial.print("genNoteTab[0]: ");
Serial.println(genNoteTab[0]);
Serial.println();
                
            }else if (nombreNoteJoue == compteur){   //Teste la correspondance de la dernière note de la série jouée et la dernière note générée
    
              melodieWin(); 
    
              delay(800);

             //Efface le tableau de note joué:
              for (int e = 0; e < 39; e++){
                noteJoueTab[e] = 0; 
              } 
       
             //incrémente de +1 le nombre de tour:
              compteur ++; 

             //Met à zéro le nombre des notes jouées: 
              nombreNoteJoue = 0;

             //incrémente de +1 le résultat: 
              resultat = 2; 

             //Met à zéro le tableau des sonars: 
              sonarTab[0]= 39;
              sonarTab[1]= 39;

Serial.println("win");
Serial.print("compteur: ");
Serial.println(compteur);
Serial.print("resultat: ");
Serial.println(resultat);
Serial.print("nombreNoteJoue: ");
Serial.println(nombreNoteJoue);
Serial.println();

            }else {  
                              
              resultat = 1;

              nombreNoteJoue ++;

            //Met à zéro le tableau des sonars: 
              sonarTab[0]= 39;
              sonarTab[1]= 39;         
                              
Serial.println("encore");
Serial.print("resultat: ");
Serial.println(resultat);
Serial.print("nombreNoteJoue: ");
Serial.println(nombreNoteJoue);
Serial.println();              
            }        
          }
Serial.println("break 2");
Serial.println(); 
          break;                    
        }else{
          delay(30);
        }
      }
    }    
  }
}




  

