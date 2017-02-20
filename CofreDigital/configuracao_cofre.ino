/*  Projeto Confre digital simples 
 *  Componentes: Display LCD 16X2, Teclado matricial 4X3, Buzzer e led RGB.
 *  Autor: Matheus Fernando Mollon
 *  Data: 16/02/2016
 *  Versão: 1.0
 *  Observações: este código é open-source. Logo, é permitido sua utilização e alteração, 
 *  sendo proibida venda deste projeto ou parte do mesmo. 
 */
#include <EEPROM.h> // importa a biblioteca da EEPROM
int estaFechado = 1; // cofre está fechado em uma situação inicial
int endereco = 0; // percorre os endereços para leitura e escrita na EEPROM
int senhaPadrao[6] = {0, 0, 0, 0, 0, 0}; // vetor de senha
int value; // exibir os dados da EEPROM
void setup() {

 //Gravar status padrão do cofre
  EEPROM.write(endereco, estaFechado); // grava o estado inicial do cofre
  endereco++;
  // Gravando a senha padrão

  while (endereco < 7) {
    EEPROM.write(endereco, senhaPadrao[endereco - 1]); // grava a senha padrão do cofre
    endereco++;
  }

  //Ativando serial para leitura
  Serial.begin(9600);
  endereco = 0;
  while (endereco < 7) { // percorre os endereços em que os dados foram armazenados, exibindo seus conteúdos.
    // lê o byte no endereço atual da EEPROM
    value = EEPROM.read(endereco);
    // envia o valor lido para o computador pela porta serial
    Serial.print(endereco);
    Serial.print("\t");
    Serial.print(value);
    Serial.println();
    endereco++;
  }

}

void loop() {}
