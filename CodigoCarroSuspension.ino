//------------Pines usados para el control del carro-llantas------------//
int izqA = 9;
int izqB = 10;
int derA = 11;
int derB = 12;

//------------Pines usados para el control de la suspension------------//
//  Bobina Izquierda de la Suspension
int const ENA_SUSP = 3; // controld el pwm
int const IN1 = 5;
int const IN2 = 4;
//  Motor Derecha de la Suspension
int const ENB_SUSP = 6; // controld el pwm
int const IN3 = 7;
int const IN4 = 8;

//------------Variables constantes------------//
int const MIN_SPEED = 10;    // Minima señal pwm
int const ACCEL_DELAY = 50;  // tiempo de subida y bajada
int const inc_dec_amount = 2; // contante de incremento suspensión

//------------Variables constantes de la comunicación bluetooth------------//
char bluetooth; // variable donde se guarda el valor recibido por el bluetoot del celular
bool suspension_mode;

// Variables de Suspensión
char susp_curr_direction_right;
char susp_curr_direction_left;
int susp_curr_speed_right;
int susp_curr_speed_left;

//===============================================================================
//  Inicialización y configuracion de los Pines
//===============================================================================
void setup() {
  // Moviemiento del carro
  pinMode(derA, OUTPUT);
  pinMode(derB, OUTPUT);
  pinMode(izqA, OUTPUT);
  pinMode(izqB, OUTPUT);

  // Control de la suspensión
  pinMode(ENA_SUSP, OUTPUT);  
  pinMode(ENB_SUSP, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Se inizializa el movimiento del carro y de la suspension en modo apagado, protección
  digitalWrite(derA, LOW);
  digitalWrite(derB, LOW);
  digitalWrite(izqA, LOW);
  digitalWrite(izqB, LOW);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  // Valores de inizialización de la suspension valores por defecto
  suspension_mode = false;
  susp_curr_direction_right = 'F';  // forward
  susp_curr_direction_left = 'F';   // forward
  susp_curr_speed_right = 0;        // stop
  susp_curr_speed_left = 0;         // stop

  Serial.begin(38400);  // Configuracion de la velocidad entre el arduino y el Bluetooth a 38400 Baud/s
}
//===============================================================================
//  Main
//===============================================================================
void loop() {
  // Comunicación Serial- Bluetooth
  if (Serial.available()) {
    bluetooth = Serial.read();
    Serial.println(bluetooth);
  }

  // Mdo de Suspensción elije si quiere modo carro o modo suspensión
  if (bluetooth == 'X')
    suspension_mode = true; // entra al control de la suspensión electromagnetica
  else if (bluetooth == 'x')
    suspension_mode = false; // entra al control del movimiento del carro

///------------Bloque del control de la suspensión------------//

  if (suspension_mode) {
    switch (bluetooth) {
      // La suspension se levanta
      case 'F':
        setDirAndSpeed('F', susp_curr_direction_right, susp_curr_speed_right);
        setDirAndSpeed('F', susp_curr_direction_left, susp_curr_speed_left);
        break;
        // La suspension baja
      case 'B':
        setDirAndSpeed('B', susp_curr_direction_right, susp_curr_speed_right);
        setDirAndSpeed('B', susp_curr_direction_left, susp_curr_speed_left);
        break;
        // la suspension levanta lado derecho
      case 'I':
        setDirAndSpeed('F', susp_curr_direction_right, susp_curr_speed_right);
        break;
        // la suspension baja lado derecho
      case 'J':
        setDirAndSpeed('B', susp_curr_direction_right, susp_curr_speed_right);
        break;
        // la suspension levanta lado izquierdo
      case 'G':
        setDirAndSpeed('F', susp_curr_direction_left, susp_curr_speed_left);
        break;
        // la suspension baja lado izquierdo
      case 'H':
        setDirAndSpeed('B', susp_curr_direction_left, susp_curr_speed_left);
        break;

      // Para, resetea el valor
      case 'W':
      case 'w':
        susp_curr_direction_right = 'F';  // Levanta
        susp_curr_speed_right = 0;        // para
        susp_curr_direction_left = 'F';   // Levanta
        susp_curr_speed_left = 0;         // para
        break;

      default:
        break;
    }
    // aplica el estado de selección
    DriverMotor('A', susp_curr_direction_right, susp_curr_speed_right);
    DriverMotor('B', susp_curr_direction_left, susp_curr_speed_left);
  }

///------------Bloque del control de la controldel carro------------//
  else {
    switch (bluetooth) {
      // El carro queda en detenido, parado
      case 'S':
        digitalWrite(derB, LOW);
        digitalWrite(izqB, LOW);
        digitalWrite(derA, LOW);
        digitalWrite(izqA, LOW);
        break;
        // EL carro avanza 
      case 'F':
        digitalWrite(derB, LOW);
        digitalWrite(izqB, LOW);
        digitalWrite(derA, HIGH);
        digitalWrite(izqA, HIGH);
        break;
        // EL carro retrocede
      case 'B':
        digitalWrite(derA, LOW);
        digitalWrite(izqA, LOW);
        digitalWrite(derB, HIGH);
        digitalWrite(izqB, HIGH);
        break;
        // El carro gira a la derecha
      case 'R':
        digitalWrite(derB, HIGH);
        digitalWrite(izqB, LOW);
        digitalWrite(derA, LOW);
        digitalWrite(izqA, HIGH);
        break;
        // El carro gira a la Inquierda
      case 'L':
        digitalWrite(derB, LOW);
        digitalWrite(izqB, HIGH);
        digitalWrite(izqA, LOW);
        digitalWrite(derA, HIGH);
        break;
      default:
        break;
    }
  }
  delay(10);
}

///------------Bloque del control de los movimientos de la suspension------------//
// alzado, reposo, izquierdo o derecho
void DriverMotor(char mot, char dir, int speed) {
  // remap the speed from range 0-100 to 0-255
  int newspeed;
  if (speed == 0)
    newspeed = 0;  // Don't remap zero, but remap everything else.
  else
    newspeed = map(speed, 1, 100, MIN_SPEED, 255);

  switch (mot) {
    case 'A':  // Controlling Motor A
      if (dir == 'F') {
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
      } else if (dir == 'R') {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
      }
      analogWrite(ENA_SUSP, newspeed);
      break;

    case 'B':  // Controlling Motor B
      if (dir == 'F') {
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
      } else if (dir == 'R') {
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
      }
      analogWrite(ENB_SUSP, newspeed);
      break;

    case 'C':  // Controlling Both Motors
      if (dir == 'F') {
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
      } else if (dir == 'R') {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
      }
      analogWrite(ENA_SUSP, newspeed);
      analogWrite(ENB_SUSP, newspeed);
      break;
  }
}


///------------Bloque del control pwm y altura de la suspensión------------//
void setDirAndSpeed(char requested_dir, char &curr_direction, int &curr_speed) {
  switch (requested_dir) {
    // La suspension se levanta
    case 'F':
      if (curr_direction == 'F' && 100 - inc_dec_amount < curr_speed)
        curr_speed = 100;

      else if (curr_direction == 'F' && 0 <= curr_speed)
        curr_speed += inc_dec_amount;

      else if (curr_direction == 'R' && curr_speed <= MIN_SPEED + inc_dec_amount) {
        curr_direction = 'F';  // forward
        curr_speed = 0;
      }

      else
        curr_speed -= inc_dec_amount;
      break;
    // La suspensio baja
    case 'B':
      if (curr_direction == 'R' && 100 - inc_dec_amount < curr_speed)
        curr_speed = 100;

      else if (curr_direction == 'R' && 0 <= curr_speed)
        curr_speed += inc_dec_amount;

      else if (curr_direction == 'F' && curr_speed <= MIN_SPEED + inc_dec_amount) {
        curr_direction = 'R';  // forward
        curr_speed = 0;
      }
      else
        curr_speed -= inc_dec_amount;
      break;

    default:
      // preserve current state
      break;
  }
}
