/*Project: enceinte thermique à élelemnt Peltier 
By: Ahmed MJADI
UTBM 20-21
*/


#include <SimpleTimer.h>
SimpleTimer timer;                 // Timer pour échantillonnage
unsigned int tick_codeuse = 0;     // Compteur de tick de la codeuse
const int frequence_echantillonnage = 50;  // Fréquence d'exécution de l'asservissement 20ms
int cmd = 0;                       // Commande du peltier
float somme_err = 0,   err_prec = 0, d_err = 0;
float Tmp = 0, T = 5 , err = 0; //T Consigne de température
const int peltier =  11;            // Digital pin pour commande peltier
const int resistance = A1;
int pinDir = 13;
int   kp = 250; //COEFFICIENT de proportionnalité
float ki = 1.05;           // Coefficient intégrateur
int kd = 20;           // Coefficient dérivateur

void setup() {
  Serial.begin(9600);         // Initialisation port COM
  // Améliore la précision de la mesure de température en réduisant la plage de mesure
  analogReference(INTERNAL); // Pour Arduino UNO
  // analogReference(INTERNAL1V1); // Pour Arduino Mega2560
  pinMode(peltier, OUTPUT);   // Sortie peltier
  analogWrite(peltier, 255);  // Sortie peltier à 0
  delay(1000);                
  attachInterrupt(0, compteur, CHANGE);    // Interruption sur tick de la codeuse  (interruption 0 = pin2 arduino mega)
  timer.setInterval(1000 / frequence_echantillonnage, asservissement); // Interruption pour calcul du P et asservissement
  Serial.println("Tmp T err cmd");
}
/* Fonction principale */
void loop() {
  timer.run();
  delay(10);
}
/* Interruption sur tick de la codeuse */
void compteur() {
  tick_codeuse++;  // On incrémente le nombre de tick de la codeuse
}
/* Interruption pour calcul du P */
void asservissement()
{
  // Mesure la tension sur la broche A0
  int valeur_brute1 = analogRead(A0);
  delay(10);
  int valeur_brute2 = analogRead(A0);
  delay(10);
  int valeur_brute3 = analogRead(A0);
  int valeur_brute = ( valeur_brute1 + valeur_brute2 + valeur_brute3) / 3;
  // Transforme la mesure (nombre entier) en température via un produit en croix
  float Tmp = valeur_brute * (1.1 / 1023.0 * 100.0);
  // Calcul de l'erreur
  int frequence_codeuse = frequence_echantillonnage * tick_codeuse;
  float err = Tmp - T;  //Tmp mesurée , T consigne
  somme_err += err;
  float d_err = err - err_prec;
  err_prec = err;
  tick_codeuse = 0;
  // PID : calcul de la commande
  float     cmd = kp * err + ki * somme_err + kd * d_err;

  // Normalisation et contrôle de température
  if (cmd < 0) { //actionner  la résistance chauffante
    if (cmd < -230)cmd = 230;
    analogWrite(resistance, cmd);
  }

  else if (cmd > 0) {   //actionner le Peltier
    if (cmd > 230) cmd = 230;
    digitalWrite(pinDir, LOW);
    analogWrite(peltier, cmd);
  }
  // Envoi la mesure au PC pour affichage et attends 10ms
  Serial.print(Tmp);
  Serial.print(",");
  Serial.print(T);
  Serial.print(",");
  Serial.print(err);
  Serial.print(",");
  Serial.print(cmd / 100);
  Serial.println(" ");
  delay(250);
}
