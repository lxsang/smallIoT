Class Scheduler Object notdone processList currentProcess
Methods Scheduler 'all'
	new
		"create a new scheduler with empty process list "
		notdone <- true.
		processList <- Set new.

|

	run
		" run as long as process list is non empty "
		[ notdone ] whileTrue:
			[ processList size = 0 ifTrue: 
				[ self initialize ].
			  processList do: 
				[ :x | currentProcess <- x.
					x execute  ] ]

|

	currentProcess
		" return the currently executing process "
		^ currentProcess

|

	yield
		" set time slice counter to zero, thereby
		yielding to next process "
		<53 0>

|

	addProcess: aProcess
		" add a process to the process list "
		processList add: aProcess

|

	removeProcess: aProcess
		" remove a given process from the process list "
		processList remove: aProcess.

|
	initialize	| string |
		<2>.
		stdout printNoReturn: '>>> '.
		string <- stdin getString.
		string isNil
			ifTrue: [ notdone <- false ]
			ifFalse: [ (string size > 0)
				ifTrue: [ 
					[ string doIt print ] fork ] ]
|

	critical: aBlock
		"set time slice counter high to insure bytecodes are
		executed before continuing "
		<53 10000>.
		aBlock value.
		"then yield processor "
		<53 0>.

]
