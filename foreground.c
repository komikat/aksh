int foreground = 0; // if a foreground process is running

void switch_ground() {
    // switches to foreground
    foreground = (foreground + 1) % 2;
}
