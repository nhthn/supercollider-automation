Automation : UGen {
	*ar { |time, values, durations|
		var args;
		if(durations.size != (values.size - 1)) {
			Error("durations must be one less in size than values").throw;
		};
		args = (
			[this.ensureAudioRate(time)]
			++ [values.size]
			++ values.collect { |value| this.ensureAudioRate(value) }
			++ durations.collect { |duration| this.ensureAudioRate(duration) }
		);
		^this.multiNew(\audio, *args);
	}
	*ensureAudioRate { |thing|
		if(thing.rate == \audio) {
			^thing;
		};
		^K2A.ar(thing);
	}
}
