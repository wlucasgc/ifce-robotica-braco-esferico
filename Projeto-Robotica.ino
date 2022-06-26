//=======================================================================================================================
//IFCE Campus Limoeiro do Norte / CE
//Curso:      Tecnologia em Mecatrônica Industrial
//Disciplina: Robótica I (S5)
//Projeto:    Braço Esférico (polar)
//Equipe:     André Luis de Oliveira Targino,
//            Gideão Ferreira de Sousa,
//            José Daniel da Silva, 
//            Marlon da Silva Maia,
//            Weber Lucas Guimarães Chaves
//=======================================================================================================================

//=======================================================================================================================
//BIBLIOTECAS
//=======================================================================================================================

#include <Servo.h>                      //Biblioteca para controle dos Servo Motores
#include <Stepper.h>                    //Biblioteca para controle do Motor de Passo

//=======================================================================================================================
//DEFINIÇÕES
//=======================================================================================================================

#define STEPPER_DIR  2                  //Porta ligada ao pino DIR do Driver A4988
#define STEPPER_STEP 3                  //Porta ligada ao pino STEP do Driver A4988

#define SERVO_BASE_PIN  4               //Porta ligada ao pino de sinal do Servo Motor da base
#define SERVO_BRACO_PIN 5               //Porta ligada ao pino de sinal do Servo Motor do braço
#define SERVO_GARRA_PIN 6               //Porta ligada ao pino de sinal do Servo Motor da garra

#define BOTAO_FC_PRISM_PIN 7            //Porta ligada ao botão indicador de fim de curso do motor de passo

#define POT_BASE_PIN  A1                //Porta ligada ao potenciômetro responsável por controlar o Servo Motor da base
#define POT_BRACO_PIN A2                //Porta ligada ao potenciômetro responsável por controlar o Servo Motor do braço
#define POT_PRISM_PIN A3                //Porta ligada ao potenciômetro responsável por controlar o Motor de Passo da junta prismática
#define POT_GARRA_PIN A4                //Porta ligada ao potenciômetro responsável por controlar o Servo Motor da garra

#define STEPPER_PASSOS_POR_VOLTA 800    //Quantidade de passos que o motor de passos deve realizar para dar 1 volta completa
#define STEPPER_PASSO  5                //De quantas em quantas voltas o motor de passo irá girar
#define SERVOS_PASSO   3                //De quantos em quantos graus será o incremento/decremento dos ângulos dos servos
#define FUSO_MAX_VOLTAS 100             //Quantidade máxima de voltas que o motor de passo pode dar no fuso
#define LEITURAS 5                      //Quantidade de vezes que as portas analógicas devem ser lidas (evitar a execução de um rúido)

//=======================================================================================================================
//OBJETOS
//=======================================================================================================================

Servo servo_base;                        //Servo Motor da base
Servo servo_braco;                       //Servo Motor do braço
Servo servo_garra;                       //Servo Motor da garra

Stepper stepper_prism(STEPPER_PASSOS_POR_VOLTA, STEPPER_DIR, STEPPER_STEP);   //Motor de passo (junta prismática)

//=======================================================================================================================
//VARIÁVEIS GLOBAIS
//=======================================================================================================================

int base,         //Variáveis que
    braco,        //guardam os valores
    prism,        //atuais de cada
    garra;        //motor

//=======================================================================================================================
//FUNÇÕES
//=======================================================================================================================

void controles_setup();

void servo_base_setup();
void servo_base_loop();

void servo_braco_setup();
void servo_braco_loop();

void servo_garra_setup();
void servo_garra_loop();

void stepper_setup();
void stepper_loop();

void stepper_voltas(float voltas);

bool stepper_fim_de_curso();

void debug();

//=======================================================================================================================
//SETUP
//=======================================================================================================================

void setup(){
    Serial.begin(9600);         //Inicia a comunicação Serial

    Serial.println("************************************************");
    Serial.println("IFCE Campus Limoeiro do Norte / CE");
    Serial.println("Curso:      Tecnologia em Mecatrônica Industrial");
    Serial.println("Disciplina: Robótica I (S5)");
    Serial.println("Projeto:    Braço Esférico (polar)");                 //Apresentação do trabalho e equipe
    Serial.println("Equipe:     André Luis de Oliveira Targino,");
    Serial.println("            Gideão Ferreira de Sousa,");
    Serial.println("            José Daniel da Silva, ");
    Serial.println("            Marlon da Silva Maia,");
    Serial.println("            Weber Lucas Guimarães Chaves");
        
    Serial.println("************************************************");
    Serial.println("Iniciando...");
    Serial.println("************************************************");
    delay(500);
    
    servo_base_setup();
    servo_braco_setup();        //Configura os componentes
    servo_garra_setup();
    stepper_setup();
    controles_setup();
    
    Serial.println("************************************************");    
    Serial.println("Tudo pronto!");
    Serial.println("************************************************");
    delay(500);
}

//=======================================================================================================================
//LOOP
//=======================================================================================================================

void loop(){
    stepper_loop();
    servo_base_loop();
    servo_braco_loop();
    servo_garra_loop();  
    
    //debug();   
    
    /*
    char c = ' ';    
    String comando = "";
    int voltas = 0; 
    bool executar = false;

    while(Serial.available()) {       //Verifica a Serial
        c = Serial.read();            //Lê a Serial caracter por caracter

        if(c != '\n') {               //Se o caracter for diferente de '\n', grava o comando
            comando += c;             
        }
        else {                        //Se o caracter for '\n', chegou ao final do comando
            executar = true;
        }
    }

    if(executar) {                                          //Executa o comando recebido
        Serial.println("Comando recebido: " + comando);
        extrairValores(comando);                            //(um comando válido possui 21 caracteres)

        voltas = comando.toInt();

        Serial.println("Voltas:  " + String(voltas));
        Serial.println("Sentido: " + (voltas >= 0) ? ("Horário") : ("Anti-horário"));
        
        stepper_volta(voltas);

        Serial.println("************************************");
              
    }

    suavizar_servo(servo1, servo1_angulo);
    
    delay(10);
    */
}

//=======================================================================================================================
//FUNÇÕES
//=======================================================================================================================

//=======================================================================================================================
//CONFIGURA OS CONTROLES (POTENCIÔMETROS)
//=======================================================================================================================

void controles_setup() {
    pinMode(POT_BASE_PIN, INPUT);
    pinMode(POT_BRACO_PIN, INPUT);      //Define os pinos
    pinMode(POT_PRISM_PIN, INPUT);      //como Input
    pinMode(POT_GARRA_PIN, INPUT);
    Serial.println("Controles OK");
    delay(500);
}

//=======================================================================================================================
//CONFIGURA O SERVO MOTOR DA BASE
//=======================================================================================================================

void servo_base_setup() {
    servo_base.attach(SERVO_BASE_PIN);
    Serial.println("Servo Motor da base OK");
    delay(500);
}

//=======================================================================================================================
//FUNÇÃO A SER INCLUÍDA NO LOOP PARA CONTROLAR O SERVO MOTOR DA BASE
//=======================================================================================================================

void servo_base_loop() {
    static unsigned long tempo = 0;
    int angulo = 0;
    
    //Executa a cada 100ms (10x por segundo) 
    if(millis() - tempo >= 100) {
        for(int i = 0 ; i < LEITURAS ; i++) {
            angulo = angulo + analogRead(POT_BASE_PIN);
            delayMicroseconds(100);
        }
                
        base = SERVOS_PASSO * map(angulo / LEITURAS, 0, 1023, 0, 180 / SERVOS_PASSO);
               
        servo_base.write(base);
    
        tempo = millis();
    }
}

//=======================================================================================================================
//CONFIGURA O SERVO MOTOR DO BRAÇO
//=======================================================================================================================

void servo_braco_setup() {
    servo_braco.attach(SERVO_BRACO_PIN);
    Serial.println("Servo Motor do braço OK");
    delay(500);
}

//=======================================================================================================================
//FUNÇÃO A SER INCLUÍDA NO LOOP PARA CONTROLAR O SERVO MOTOR DO BRAÇO
//=======================================================================================================================

void servo_braco_loop() {
    static unsigned long tempo = 0;
    int angulo = 0;
    
    //Executa a cada 100ms (10x por segundo) 
    if(millis() - tempo >= 100) {
        for(int i = 0 ; i < LEITURAS ; i++) {
            angulo = angulo + analogRead(POT_BRACO_PIN);
            delayMicroseconds(100);
        }
                
        braco = SERVOS_PASSO * map(angulo / LEITURAS, 0, 1023, 0, 180 / SERVOS_PASSO);
        
        servo_braco.write(braco);
    
        tempo = millis();
    }
}

//=======================================================================================================================
//CONFIGURA O SERVO MOTOR DA GARRA
//=======================================================================================================================

void servo_garra_setup() {
    servo_garra.attach(SERVO_GARRA_PIN);
    Serial.println("Servo Motor da garra OK");
    delay(500);
}

//=======================================================================================================================
//FUNÇÃO A SER INCLUÍDA NO LOOP PARA CONTROLAR O SERVO MOTOR DA GARRA
//=======================================================================================================================

void servo_garra_loop() {
    static unsigned long tempo = 0;
    int angulo = 0;
    
    //Executa a cada 100ms (10x por segundo) 
    if(millis() - tempo >= 100) {
        for(int i = 0 ; i < LEITURAS ; i++) {
            angulo = angulo + analogRead(POT_GARRA_PIN);
            delayMicroseconds(100);
        }
        
        garra = SERVOS_PASSO * map(angulo / LEITURAS, 0, 1023, 0, 180 / SERVOS_PASSO);
                
        servo_garra.write(garra);
    
        tempo = millis();
    }
}

//=======================================================================================================================
//FAZ O MOTOR DE PASSO INICIAR EM SUA POSIÇÃO ZERO (FIM DE CURSO)
//=======================================================================================================================

void stepper_setup() {
    stepper_prism.setSpeed(255);
    pinMode(BOTAO_FC_PRISM_PIN, INPUT_PULLUP);
        
    while(!stepper_fim_de_curso()) {
        stepper_voltas(-1);
    }

    Serial.println("Motor de passo OK");
    delay(500);
}

//=======================================================================================================================
//FUNÇÃO A SER INCLUÍDA NO LOOP PARA CONTROLAR O MOTOR DE PASSO
//=======================================================================================================================

void stepper_loop() {
    static unsigned long tempo = 0;
    int voltas_restantes = 0;
    
    //Executa a cada 100ms (10x por segundo) 
    if(millis() - tempo >= 100) {
        for(int i = 0 ; i < LEITURAS ; i++) {
            voltas_restantes = voltas_restantes + analogRead(POT_PRISM_PIN);
            delayMicroseconds(100);
        }
        
        voltas_restantes = STEPPER_PASSO * map(voltas_restantes / LEITURAS, 0, 1023, 0, FUSO_MAX_VOLTAS / STEPPER_PASSO);
        voltas_restantes = voltas_restantes - prism; 
        
        tempo = millis();
    }
    
    if(voltas_restantes > 0) {
        stepper_voltas(1);
        prism++;   
    }

    if(voltas_restantes < 0) {
        stepper_voltas(-1);
        prism--;   
    }

    stepper_fim_de_curso();
}

//=======================================================================================================================
//IDENTIFICA O FIM DE CURSO DO MOTOR DE PASSO E ZERA A POSIÇÃO DO MESMO
//=======================================================================================================================

bool stepper_fim_de_curso() {
    bool resultado;

    resultado = !digitalRead(BOTAO_FC_PRISM_PIN);
    
    if(resultado) {
        prism = 0;
    }
    
    return(resultado);
}

//=======================================================================================================================
//FAZ O MOTOR DE PASSO DAR A QUANTIDADE DE VOLTAS DESEJADA (POSITIVO: SENTIDO HORÁRIO | NEGATIVO: SENTIDO ANTI-HORÁRIO)
//=======================================================================================================================

void stepper_voltas(float voltas) {
    stepper_prism.step(voltas * STEPPER_PASSOS_POR_VOLTA);
}

//=======================================================================================================================
//FUNÇÃO DE DEBUG (IMPRIME OS VALORES ATUAIS DOS MOTORES)
//=======================================================================================================================

void debug() {
    Serial.println(String(base) + " " + String(braco) + " " + String(prism) + " " + String(garra));
}

//=======================================================================================================================




/*
//=======================================================================================================================
//FAZ O MOTOR DE PASSO DAR A QUANTIDADE DE VOLTAS DE ACORDO COM A POSIÇÃO DO POTENCIÔMETRO
//=======================================================================================================================

void stepper_voltas_pot() {
    static unsigned long tempo = 0;

    //Executa a cada 100ms (10x por segundo) 
    if(millis() - tempo >= 100) {
        voltas_restantes = analogRead(POT_PRISM_PIN);
        voltas_restantes = STEPPER_PASSO * map(voltas_restantes, 0, 1023, 0, FUSO_MAX_VOLTAS / STEPPER_PASSO);
        voltas_restantes = voltas_restantes - prism; 

        Serial.println(prism);

        tempo = millis();
    }
}
*/








//=======================================================================================================================
//EXTRAI OS VALORES DO COMANDO RECEBIDO E GRAVA NAS RESPECTIVAS VARIÁVEIS
//=======================================================================================================================

void extrairValores(String comando) {
    /*
    String componente = "";
    int valor;

    componente = comando.substring(0, comando.indexOf(":"));
    valor = comando.substring(comando.indexOf(":") + 1, comando.length()).toInt();

    if(componente.equals("M")) {
        Serial.println("Controlando o motor de passo (valor: " + String(valor) + ")");
    }
    
    if(componente.equals("S1")) {
        Serial.println("Controlando o servo motor 1 (valor: " + String(valor) + ")");
        
        servo1_angulo = map(valor, 0, 180, 13, 173);
    }
    */
    
    /*
    int inicio = 0,
        fim = 0;
            
    fim = comando.indexOf(":", inicio);
    direcao = comando.substring(inicio, fim);
    inicio = fim + 1;

    fim = comando.indexOf(":", inicio);
    velocidade = comando.substring(inicio, fim).toInt();
    velocidade = map(velocidade, 1, 5, 120, 255);
    inicio = fim + 1;

    fim = comando.indexOf(":", inicio);
    servo1_angulo = comando.substring(inicio, fim).toInt();
    servo1_angulo = map(servo1_angulo, 0, 180, 120, 10);
    inicio = fim + 1;

    fim = comando.indexOf(":", inicio);
    servo2_angulo = comando.substring(inicio, fim).toInt();
    servo2_angulo = map(servo2_angulo, 0, 165, 180, 0);
    inicio = fim + 1;

    fim = comando.indexOf(":", inicio);
    servo3_angulo = comando.substring(inicio, fim).toInt();
    servo3_angulo = map(servo3_angulo, 0, 180, 0, 180);
    inicio = fim + 1;

    fim = comando.indexOf(":", inicio);
    servo4_angulo = comando.substring(inicio, fim).toInt();
    servo4_angulo = map(servo4_angulo, 0, 165, 180, 0);

    Serial.println("************************************");
    Serial.println("Comando: " + comando);
    Serial.println("Direcao:    " + direcao);
    Serial.println("Velocidade: " + String(velocidade));
    Serial.println("Servo 1:    " + String(servo1_angulo));
    Serial.println("Servo 2:    " + String(servo2_angulo));
    Serial.println("Servo 3:    " + String(servo3_angulo));
    Serial.println("Servo 4:    " + String(servo4_angulo));
    */
}

//=======================================================================================================================
//MOVIMENTA O SERVO DE FORMA MAIS SUAVE
//=======================================================================================================================

void suavizar_servo(Servo servo, int angulo) {
    int angulo_atual;
    
    angulo_atual = servo.read();

    if(angulo_atual < angulo) {
        angulo_atual++;
        servo.write(angulo_atual);    
    }

    if(angulo_atual > angulo) {
        angulo_atual--;
        servo.write(angulo_atual);    
    }
}

//=======================================================================================================================
