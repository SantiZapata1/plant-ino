void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
unsigned long minutesToMillis(int minutes) {
  return minutes * 60000;
}
unsigned long minutesToSeconds(int minutes) {
  return minutes * 60;
}
unsigned long millisToSeconds(unsigned long millis) {
  return millis / 1000;
}
double secondToMinutes(int seconds) {
  return seconds / 60;
}