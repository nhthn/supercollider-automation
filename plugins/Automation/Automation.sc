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
			[\bounce, (1..8)],
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
			[this.ensureAudioRateLinearInterp(time)]
			++ [values.size]
			++ values.collect { |value| this.ensureAudioRateLinearInterp(value) }
			++ durations.collect { |duration| this.ensureAudioRateLinearInterp(duration) }
			++ easingFunctions.collect { |easingFunction| this.wrapEasingFunction(easingFunction) }
		);
		^this.multiNew(\audio, *args);
	}

	*wrapEasingFunction { |thing|
		var result;
		result = thing;
		if(result.isKindOf(Symbol)) {
			if(this.easingFunctions.includes(result).not) {
				Error("Unrecognized easing function: %".format(thing.asString)).throw;
			};
			result = this.easingFunctions.indexOf(result);
		}
		^this.ensureAudioRateNoInterp(result);
	}

	*ensureAudioRateLinearInterp { |thing|
		if(thing.rate == \audio) {
			^thing;
		};
		^K2A.ar(thing);
	}

	*ensureAudioRateNoInterp { |thing|
		if(thing.rate == \audio) {
			^thing;
		};
		^K2A.ar(Latch.ar(thing, Impulse.ar(ControlRate.ir)));
	}
}
