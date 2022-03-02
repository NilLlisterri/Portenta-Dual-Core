void setup_m7() {
  // Initialize RPC library; this also boots the M4 core
  RPC.begin();
  Serial.begin(115200);
  RPC.bind("remoteAdd", addOnM7);
}

void loop_m7() {
  String buffer = "";
  while (RPC.available()) {
    buffer += (char)RPC.read(); // Fill the buffer with characters
  }

  if (buffer.length() > 0) {
    Serial.print(buffer);
  }
}

int addOnM7(int a, int b) {
  Serial.println("M7: executing add with " + String(a) + " and " + String(b));
  return a + b;
}
