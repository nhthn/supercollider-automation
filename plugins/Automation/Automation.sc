Automation : UGen {
	*easingFunctions {
		var result;
		result = [\linear];
		[
			\step,
			\quadratic,
			\sine,
			\cubic,
			\quartic,
			\quintic,
			\pseudoExponential,
			\circular
		].do { |prefix|
			result = result ++ [
				(prefix.asString ++ "In").asSymbol,
				(prefix.asString ++ "Out").asSymbol,
				(prefix.asString ++ "InOut").asSymbol,
			];
		};
		[
			[\elastic, (1..8)],
			[\sinc, (1..8)],
			[\staircase, (2..8)],
		].do { |spec|
			var prefix, parameters;
			# prefix, parameters = spec;
			parameters.do { |parameter|
				result = result ++ [
					(prefix.asString ++ parameter.asString ++ "In").asSymbol,
					(prefix.asString ++ parameter.asString ++ "Out").asSymbol,
					(prefix.asString ++ parameter.asString ++ "InOut").asSymbol,
				];
			}
		};
		^result;
	}

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
		if(result.isKindOf(Symbol)) {
			if(this.easingFunctions.includes(result)) {
				Error("Unrecognized easing function: %".format(thing.asString)).throw;
			};
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
