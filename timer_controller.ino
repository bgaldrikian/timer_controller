const long min_on_sec = 1;
const long max_on_sec = 10;

const long min_off_sec = 5;
const long max_off_sec = 60;

const long sensor_thresh = 500;

const int led_pin = 3;
const int relay_control_pin = 8;

const int on_pot_input_pin = A0;
const int off_pot_input_pin = A4;
const int sensor_input_pin = A2;

enum State
{
  Off,
  On
};

int state = Off;

unsigned long event_time_ms = 0;
bool timing = false;
void set_event_delay(unsigned long delay_ms)
{
  event_time_ms = millis() + delay_ms;
  timing = true;
}

bool update_event_timer()
{
  long delta = (long)millis() - (long)event_time_ms;
  if (!timing || delta < 0) return false;
  timing = false;
  return true;
}

void setup()
{
  pinMode(on_pot_input_pin, INPUT);
  pinMode(off_pot_input_pin, INPUT);
  pinMode(sensor_input_pin, INPUT);
  digitalWrite(relay_control_pin, LOW);
  pinMode(led_pin, OUTPUT);
  pinMode(relay_control_pin, OUTPUT);
  set_event_delay(0);
}

void loop()
{
  const long sensor_val = analogRead(sensor_input_pin);
  const long on_pot_val = analogRead(on_pot_input_pin);
  const long off_pot_val = analogRead(off_pot_input_pin);
  const bool sensor_on = (sensor_val >= sensor_thresh);

  digitalWrite(led_pin, sensor_on ? HIGH : LOW);

  if (state == Off && !sensor_on) return;
  const long on_ms = (max_on_sec - min_on_sec) * on_pot_val + min_on_sec * 1000; // 2.3% error, but it's close enough
  const long off_ms = (max_off_sec - min_off_sec) * off_pot_val + min_off_sec * 1000; // 2.3% error, but it's close enough
 
  if (update_event_timer())
  {
    switch (state)
    {
      case Off:
        digitalWrite(relay_control_pin, HIGH);
        state = On;
        set_event_delay(on_ms);
        break;
      case On:
        digitalWrite(relay_control_pin, LOW);
        state = Off;
        set_event_delay(off_ms);
        break;
    }
  }
}

