// ? ****** BIBLIOTECAS *******
#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <U8g2lib.h>
#include <DHT.h>
#include <Bounce2.h>

// ? ****** DEFINIÇÕES *******
#define DHTTYPE DHT22       //Tipo do DHT 

// ? ****** PINAGEM *******
#define DHTPIN 4                         //Pino do DHT
#define BOTAOPIN 12                    //Pino do Botao
#define LEDPIN 5                          //Pino do LED

// ? ****** OBJETOS *******
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE); //Instancia o objeto u8g2
DHT dht(DHTPIN, DHTTYPE); //Instancia o objeto dht
Bounce2::Button botao = Bounce2::Button(); //Instancia o objeto botao

// ? ****** VARIÁVEIS *******
bool statusLamp = 0;      //variavel que armazena o estado da Lampada
bool statusBotao = 0;     //variavel que armazena o estado do botao
bool statusDHT = 1;       //variavel que armazena se o DHT está funcionando

float temperatura;        //Variavel que armazena o valor da temperatura (ºC)
float umidade;            //variavel que armazena o valor da umidade (%)

// ? ****** PROTOTIPOS DE FUNÇÕES *******
void telaFundo(void);
void atualizaBotao(void);
void atualizaDht(void);
void simbolo (char, int, int );

// ? ****** SETUP *******
void setup()
{
  u8g2.begin();       //inicia o objeto u8g2
  Serial.begin(9600); //inicia a comunicacao serial
  dht.begin();        //inicia o dht
  botao.attach(BOTAOPIN, INPUT_PULLUP); //inicia o botao (pino, tipo de conexao)
  botao.interval(5);  //tempo em ms do filtro de ruido do botao
  botao.setPressedState(LOW); //estado fisico do pino quando o botao estiver pressionado
  pinMode(LEDPIN, OUTPUT);
}

void loop()
{
  atualizaBotao();
  atualizaDht();
  telaFundo();
  digitalWrite(LEDPIN, statusLamp);
}

// ? ****** FUNÇÕES *******
//Funcao que faz a leitura do sensor DHT, 
//verifica a comunicacao, e atualiza as variaveis
void atualizaDht() 
{
  temperatura = dht.readTemperature();
  umidade = dht.readHumidity();
                        //OR
  if ( isnan(temperatura) || isnan(umidade) ) //verifica se o valor temperatura e umidade não são numeros
    statusDHT = 0;  //status que nao está funcionando

  else //caso for numeros
     statusDHT = 1;  //status que está funcionando

}

//Funcao que faz a leitura do botao
void atualizaBotao()
{
  botao.update(); //verifica o estado do botao
  statusBotao = botao.isPressed(); //atualiza o estado do botao

  if (botao.pressed()) //o botão foi pressioando?
  {
    statusLamp = !statusLamp; //inverte oe estado da lampada
  }

  if (botao.released()) //o botao foi solto? 
  {
    unsigned long tempo = botao.previousDuration(); //quanto tempo ele ficou pressioando
  }
}


//Funcao que atualiza o display
void telaFundo()
{
  u8g2.clearBuffer(); //apaga o buffer para construir uma nova tela

  //Retangulo de superior esq.
  u8g2.drawRFrame(0, 0, 63, 31, 5); //retangulo sem preenchimento
  simbolo('t', 3, 24); //icone do termometro

  if (statusDHT) //DHT está funcionando?
  {
    u8g2.setFont(u8g2_font_courB10_tf); //muda para fonte de texto
    u8g2.setCursor(24, 24); //posiciona o cursor
    u8g2.print(temperatura, 0); //imprime o valor da temperatura sem casa decimal
    u8g2.write(0xb0); //imprime o simbolo de º
    u8g2.print("C");
  }
  else  //DHT não está funcionando
  {
    simbolo('f', 30, 24);
  }

  //Retangulo de superio dir
  u8g2.drawRFrame(65, 0, 63, 31, 5); //retangulo sem preenchimento
  simbolo('u', 68, 24); //icone de umidade?

  if (statusDHT) //DHT está funcionando?
  {
    u8g2.setFont(u8g2_font_courB10_tf); //muda para fonte de texto
    u8g2.setCursor(92, 24); //posiciona o cursor
    u8g2.print(umidade, 0); //imprime o valor da umidade sem casa decimal
    u8g2.print("%");
  }

  else //DHT não está funcionando
  {
    simbolo('f', 98, 24);
  }

  //Retangulo de inferior esq.
  if (statusBotao) //o botão está pressionado?
  {
    u8g2.drawRBox(0, 33, 63, 31, 5); //retangulo preenchido
    u8g2.setDrawColor(0); //inverte a cor
    simbolo('1', 21, 58); //imprime o icone do botao pressionado
    u8g2.setDrawColor(1); //desinverte a cor
  }
  else //botão solto
  {
    u8g2.drawRFrame(0, 33, 63, 31, 5);  //retangulo sem preenchimento
    simbolo('0', 21, 58); //imprime o icone do botao solto
  }

  //Retangulo de inferior dir.
  if (statusLamp) //lampada ligada?
  {
    u8g2.drawRBox(65, 33, 63, 31, 5); //retangulo preenchido
    u8g2.setDrawColor(0); //inverte a cor
    simbolo('l', 68, 57); //imprime o icone da lampada

    u8g2.setDrawColor(1); //desinverte a cor
  }

  else //lampada apagada? 
  {
    u8g2.drawRFrame(65, 33, 63, 31, 5); //Retangulo de inf. esq.
    simbolo('l', 68, 57); //imprime o icone da lampada
  }
  u8g2.sendBuffer(); //envia a tela construida em buffer
}



//Função que cria atalho para a fonte de simbolos
//https://github.com/olikraus/u8g2/wiki/fntgrpstreamline#streamline_all
void simbolo (char simbolo, int x, int y) //nao retorna nada, mas pede 3 paramentros
{
  u8g2.setFont(u8g2_font_streamline_all_t); //Muda para fonte de simbolos
  switch (simbolo)
  {
    case 't': //Temperatura
      u8g2.drawGlyph(x, y, 0x2BC);
      break;

    case 'u': //Umidade
      u8g2.drawGlyph(x, y, 0x2BD);
      break;

    case 'f': //fantasma
      u8g2.drawGlyph(x, y, 0xFD);
      break;

    case '0': //botao off
      u8g2.drawGlyph(x, y, 0x1EA);
      break;

    case '1': //botao on
      u8g2.drawGlyph(x, y, 0x1EB);
      break;

    case 'l': //lampada
      u8g2.drawGlyph(x, y, 0x18E);
      break;

  }

}

