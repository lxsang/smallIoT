Class GPIO Object mode port
Methods GPIO 'all'
	asString	| text|
		text <- ('GPIO:', (port asString), ' mode:', (self mode asString)).
		^ text
|
	setPin:aPin
		port <- aPin
|
	pin: aPin mode:aMode
		port<- aPin.
		self pinMode:aMode
|
	pinMode:aMode
		port isNil ifTrue:[^smalltalk error:'no port select'].
		<135 port aMode>
|
	input
		self pinMode:1
|
	output
		self pinMode:0
|
	inputPullup
		self pinMode:2
|
	inputPulldown
		self pinMode:3

|
	new
		mode <- Dictionary new.
		mode at:0 put:'OUTPUT'.
		mode at:1 put:'INPUT'.
		mode at:2 put:'INPUT_PULLUP'.
		mode at:3 put:'INPUT_PULLDOWN'.
		port <- nil
|
	read
		port isNil ifTrue:[^smalltalk error:'no port select'].
		^ <136 port>
|
	write:aBool
		port isNil ifTrue:[^smalltalk error:'no port select'].
		^ <137 port aBool>
]