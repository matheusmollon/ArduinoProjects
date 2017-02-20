/*  Projeto Confre digital simples 
 *  Componentes: Display LCD 16X2, Teclado matricial 4X3, Buzzer e led RGB.
 *  Autor: Matheus Fernando Mollon
 *  Data: 16/02/2016
 *  Versão: 1.0
 *  Observações: este código é open-source. Logo, é permitido sua utilização e alteração, 
 *  sendo proibida venda deste projeto ou parte do mesmo. 
 */

#include <EEPROM.h> //importa a biblioteca para manipulação da EEPROM
#include <LiquidCrystal.h> ////importa a biblioteca para manipulação do display LCD
#define numeroLinhas 4 // 4 linhas presentes no teclado
#define numeroColunas 3 // 3 colinas presentes no teclado
#define ledGreen 9 // definição do pino para o LED verde do RGB
#define ledRed 10 // definição do pino para o LED verde do RGB
#define buzzer 11 // definição do pino para o Buzzer
LiquidCrystal lcd(19, 18, 14, 15, 16, 17); // pinos do display LCD RW/EN/D0/D1/D2/D3
const char teclado[numeroLinhas][numeroColunas] = { //matriz com o teclado matricial utilizado
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

const int linhas[numeroLinhas] = { 7, 2, 3, 5}; //pinos que correspondem às linhas do teclado
const int colunas[numeroColunas] = { 6, 8, 4}; //pinos  que correspondem às colunas do teclado
char nenhuma = 0; // indica que não há uma tecla válida pressionada no teclado
int password[6]; //armazenamento da senha do usuário
int passwordAux[6] = {0, 0, 0, 0, 0, 0}; //vetor auxiliar para verificação da senha de desbloqueio do cofre
int estaFechado = 0; // indica se o cofre está trancado (1) ou se está aberto (0).

void setup() {
  /*Configuração dos pinos dos LEDS e do buzzer */
  pinMode(ledGreen, OUTPUT);
  pinMode(ledRed, OUTPUT);
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);

  /*Configuração dos pinos do teclado matricial*/
  //configura as linhas do teclado
  for (int i = 0; i < numeroLinhas; i++) {
    pinMode(linhas[i], INPUT); // as linhas são entradas digitais para o Arduino
    digitalWrite(linhas[i], HIGH); //são inicializadas em nível lógico alto
  }
  //configura as colunas como saídas com padrão em nível lógico alto
  for (int j = 0; j < numeroColunas; j++) {
    pinMode(colunas[j], OUTPUT); // as colunas são saídas digitais para o Arduino
    digitalWrite(colunas[j], HIGH); // são inicializadas com o nível lógico alto
  }

  //Inicialização e configuração do Display LCD
  lcd.begin(16, 2); //inicia o display e indica o seu tipo (16X2)
  lcd.setCursor(0, 0); //posiciona o cursos na primeira coluna e linha
  lcd.print("SAFE PROJECT");
  lcd.setCursor(0, 1);
  lcd.print("BY MOLLON");
  delay(3000);
  lcd.setCursor(0, 1);
  lcd.print("Loading...");
  delay(3000);
  lerDadosEEPROM(); // realiza a leitura da EEPROM do Arduino.


}

/*Função responsável pela leitura dos dados armazenados na EEPROM. Ao todo, são  
 * sete endereços utilizados, sendo o primeiro para armazenar o esta do
 * cofre, ou seja, está travado ou aberto e os outros 6 endereços armazenam
 * a senha definida pelo usuário.
 */
void lerDadosEEPROM() {
  for (int i = 0; i < 7; i++) {
    if (i == 0) {
      estaFechado = EEPROM.read(i);
    } else {
      password[i - 1] = EEPROM.read(i);
    }
  }
}

/*Função responsável pela escrita dos dados na EEPROM. Ao todo, são  
 * sete endereços utilizados, sendo o primeiro para armazenar o esta do
 * cofre, ou seja, está travado ou aberto e os outros 6 endereços armazenam
 * a senha definida pelo usuário.
 */
void escreverDadosEEPROM() {
  for (int i = 0; i < 7; i++) {
    if (i == 0) {
      EEPROM.write(i, estaFechado);
    } else {
      EEPROM.write(i, password[i - 1]);
    }
  }
}


void loop() {

  if (estaFechado) { // se o cofre estiver fechado ("estaFechado ==1"), então: 
    lcd.clear(); //limpa os escritos do display LCD
    digitalWrite(ledGreen, LOW); // desativa o LED verde
    digitalWrite(ledRed, HIGH); // ativa o led vermelho
    lcd.setCursor(0, 0); //posiciona o cursor em 0x0
    lcd.print("SAFE IS LOCKED");
    lcd.setCursor(0, 1);
    lcd.print("KEY:");
    lcd.print(" ");
    inserirSenha(1); //entra na função para inserção da senha, onde o sistema aguarda o usuário digitar a senha para desbloqueio do cofre.
    verificarSenha(); // verifica se a senha digitada está correta.
    if (!estaFechado) { // caso esta variável esteja com seu valor igual a zero, significa que a senha digitada está correta.
      EEPROM.write(0, estaFechado);// é sobrescrito no endereço zero da EEPROM que o cofre está liberado. 
    } else { // Se mesmo após a verificação da senha o cofre continua fechado, então a senha digitada está errada. 
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("SAFE IS LOCKED");
      lcd.setCursor(1, 4);
      lcd.print("WRONG KEY");
      delay(3000);
    }
  } else { // se o cofre não estiver fechado ("estaFechado ==0"), então:
    digitalWrite(ledRed, LOW); //desliga o led vermelho
    digitalWrite(ledGreen, HIGH); // liga o led verde
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SAFE IS UNLOCKED");
    lcd.setCursor(0, 1);
    lcd.print("KEY:");
    lcd.print(" ");
    inserirSenha(2); // esta função aguarda que uma nova senha seja configurada (digitada) para bloquear o cofre.
    estaFechado = 1; // Após a digitação da senha pelo usuário, o cofre é travado
    escreverDadosEEPROM(); // Escreve o status do cofre, bem como a nova senha na EEPROM.
  }


}

/* Função utilizada para fazer a verificação da senha inserida
 *  pelo usuário para desbloqueio do cofre. 
 */
void verificarSenha() { 
  estaFechado = 0; // assume que o cofre esteja aberto (só como critério de verificação simples)
  for (int i = 0; i < 6; i++) {
    if (password[i] != passwordAux[i]) { // caso algum elemendo do vetor da senha esteja errado
      estaFechado = 1; // mantem o estado do cofre como bloquado.
    }
  }
}

/* Esta função é responsável por aguardar a entrada da senha pelo usuário.
 *  Argumento: int com a opção de entrada.
 *             1 - para desbloquear o cofre 
 *             2-  para configurar a senha e bloquear o cofre
 * 
 */
void inserirSenha(int op) {
  int contDigitos = 0; // contador de dígitos inseridos pelo usuário
  char tecla = nenhuma; // inicia a variável tecla com nenhuma tecla válida 
  while (contDigitos < 6) { // enquanto não forem digitados os 6 digítos da senha:
    tecla = obtemTecla(); // faz a varredura no reclado matricial para obter a tecla digitada
    if (tecla != nenhuma) { // verifica se é uma tecla válida. 
      if (tecla >= '0' && tecla <= '9') { //verifica se é um valor válido
        lcd.print(tecla); // imprime o caractere digitado no display
        if (op == 1) { 
          passwordAux[contDigitos] = charToInt(tecla); // converte a tecla digitada do tipo char para seu valor inteiro.
         } else {
          password[contDigitos] = charToInt(tecla);
        }
        contDigitos++; // incrementa o total de teclas digitadas.
      }
    }
  }
}

/* Esta função realiza a conversão de um caractere (char) para um valor inteiro
 *  Argumento: char (dígito obtido pelo teclado)
 *  Retorno: int (valor convertido)
 */

int charToInt(char digito) {
  return digito - '0';
}


/* Função responsável por fazer a varredura do teclado em busca da tecla
 *  pressionada. Esta função foi extraída do material da Robocore, porém há
 *  várias formas de fazer, sendo possível encontrar bibliotecas prontas para 
 *  fazer a leitura do teclado, como a Keypad Library for Arduino que está 
 *  disponível em: http://playground.arduino.cc/Code/Keypad. Pessoalmente, 
 *  prefiro montar as lógicas para entender o funcionamento dos componentes 
 *  e escolhi a lógica da robocore, pois a maneira como é feita a varredura é 
 *  semelhante a lógica simples para percorrer matrizes na linguagem C, o que 
 *  facilita o entendimento.
 */
char obtemTecla() { 
  char tecla = nenhuma; // inicia a variável tecla
  boolean teclaPressionada = false; // variável utilizada para indicar que a tecla digitada fora encontrada
  for (int j = 0; j < numeroColunas; j++) { //percorre todas as colunas do teclado
    digitalWrite(colunas[j], LOW); 
    for (int i = 0; i < numeroLinhas; i++) { // percorre todas as linhas do teclado
      if (digitalRead(linhas[i]) == LOW) { // verifica qual linha foi pressionada pela primeira vez
        delay(30); //aplica debounce para evitar ruído
      }
      if (digitalRead(linhas[i]) == LOW) { // se ainda estiver pressionada, então de fato é uma tecla pressionada
        while (digitalRead(linhas[i]) != HIGH); // enquanto estiver pressionada nada será realizado
        tecla = teclado[i][j]; // atribui a variável "tecla" o caractere digitado através da matriz do teclado.
        digitalWrite(buzzer, HIGH); // ativa o buzzer por 50ms para indicar o sinal sonoro ao pressionar a tecla.
        delay(50);
        digitalWrite(buzzer, LOW);
        teclaPressionada = true;
        break; // sai do laço interno pois encontrou a tecla pressionada
      }
    }
    digitalWrite(colunas[j], HIGH);
    if (teclaPressionada) { // caso a tecla seja encontrada, sai do laço mais externo e retorna a tecla.
      break;
    }
  }
  return tecla;
}
