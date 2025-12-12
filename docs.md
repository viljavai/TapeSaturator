# Tape loop simulator

### Plan is to have some general tape emulation effects such as saturation and pitch variation (slider) + perhaps some funky looping functionality with tape loop -like artefacts + speed variation.

## Progress

15.11.2025  

- Started out
- Initial saturation with asymmetric waveshaping + added harmonics
- Volume increases with level of saturation (might not be an issue)

Next: Start out on pitch bend LFO, white noise slider

16.11.2025

- Issues with aliasing when adding harmonics in saturation, implemented oversampling to help with this -> LP filtering maybe also todo?
Since we are dealing with a nonlinear effect, we get aliasing probelms.
    - Adding a bias to tanh function helped with a lot
- Read up on tape hysteresis

Next: Make bias into a slider, add LP filter?

19.11
- Noise slider
- Bias is now hardcoded at 1.2, this could scale with gain in saturation, but how?

Next: Add LFO for pitch, speed?
