Automation : UGen {
	classvar <>easingFunctions = #[
		\linear,
		\firstValue,
		\nextValue,
		\quadraticIn,
		\quadraticOut,
		\quadraticInOut,
		\cubicIn,
		\cubicOut,
		\cubicInOut,
		\sineIn,
		\sineOut,
		\sineInOut,
		\quarticIn,
		\quarticOut,
		\quarticInOut,
		\quinticIn,
		\quinticOut,
		\quinticInOut,
		\pseudoExponentialIn,
		\pseudoExponentialOut,
		\pseudoExponentialInOut,
	];

	*ar { |time, values, durations, easingFunctions|
		var args;
		if(values.size < 2) {
			Error("There must be at least 2 values").throw;
		};
		if(durations.size != (values.size - 1)) {
			Error("durations must be one less in size than values").throw;
		};
		if(easingFunctions.size != (values.size - 1)) {
			Error("easingFunctions must be one less in size than values").throw;
		};
		args = (
			[this.ensureAudioRate(time)]
			++ [values.size]
			++ values.collect { |value| this.ensureAudioRate(value) }
			++ durations.collect { |duration| this.ensureAudioRate(duration) }
			++ easingFunctions.collect { |easingFunction| this.wrapEasingFunction(easingFunction) }
		);
		^this.multiNew(\audio, *args);
	}

	*wrapEasingFunction { |thing|
		var result;
		result = thing;
		if(thing.isKindOf(Symbol)) {
			result = this.easingFunctions.indexOf(result);
		}
		^this.ensureAudioRate(result);
	}

	*ensureAudioRate { |thing|
		if(thing.rate == \audio) {
			^thing;
		};
		^K2A.ar(thing);
	}
}
