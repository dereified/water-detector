Engineering Design
==================

A basic water detection circuit is trivial. Water has resistivity which, with practical electrodes places its resistance in a detectable range. All that is needed is an amplifier (such as a bipolar transistor).

In this case I faced a number of conditions:

* purpose of circuit is to make evident and alert water in obscure location
* location only has battery power available
* sump is usualy dry and needs to alert when not
* area has high footfall, but from busy people: few visitors the opportunity to fix.
* water ingress is slow (hours to days)
* area may be (rarely) unoccupied for a week or two
* underlying ingress problem is tedious and few will be proactive
* the circuit will need to be reliable
* it's a "one off", not a prototype for production

Current consumption is the greatest constraint. If the circuit doesn't have a battery lifetime of a year or so, it will be neglected. With practical batteries this means single-digit micro-amp amortised consumption.

The trivial circuit has very low current consumption, but if we are to choose anything more fancy, we need to be careful.

If a light or sounder is used, there's a real chance that a continuous alert would drain batteries before the problem can be detected/fixed. With, say 1000mAh of charge, a 20mA drain (LED) would give under two days of alert, 50mA (LED+sounder) under a day. Typically this would be fine, but not reliably so.

As the sounder can activate at any time, a continuous tone could also be irritating out of hours for what is a non-urgent condition. A constant LED is less easy to detect than flashing.

All of these constraints suggest a pulsing circuit. At this point, an MCU is probably a more trivial solution than (say) a discrete multivibrator, given the current constraints, and gives opportunities for more complex behaviours. A sleeping TinyAT85 has very low current consumption ~5uA.

If I'm to use an MCU, I may as well have the MCU use its ADC to detect the resistance.

As ingress is slow, there is the opportunity for preventative maintenance (alongside repair maintenance) of having a second, lower level of detection. This allows repair maintenance to be more assertive (buzzer) while reducing the likelihood of the buzzer sounding (the water being removed when the preventative watermark is met, without a sounder).

Battery condtion can be tested with a battery/lamp/circuit test switch but it's likely this task will be neglected. An infrequently flashing "operating" light is more likely to get attention, though care must be taken that this doesn't become a significant current draw.

Detecting discharge across multiple battery chemistries is difficult so cannot be relied upon here, but can be useful as an adjunct to the "operating" light, especially since this can be achieved entirely in software in the ATTiny85.

As the circuit is a straight-to-deployment prototype which needs to be reliable, it makes sense to be conservative in the design and engineering tolerances. To this end,

* although the MCU can switch significant loads, to reduce the chances of brownout it makes sense to buffer the pins with a transistor stage: this costs pennies.

* as the buzzer is electromechanical should be protected by a flyback diode, even though no significant spikes were found.

* the inputs should be protected by small-value resistors in case they ever end up misconfigured as outputs (eg during startup or transient conditions)!

* the MCU should be decoupled, even though it rarely needs it.

* given little chance of extendsive pre-deployment testing under realistic conditions, a debug mode should be provided to allow conditions (such as unexpected resistance values) to be detected after deployment.
