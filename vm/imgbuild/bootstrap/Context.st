*Class Context Object linkLocation method arguments temporaries
Methods Context 'all'
	at: key put: value
		temporaries at: key put: value
|
	method: m
		method <- m
|
	arguments: a
		arguments <- a
|
	temporaries: t
		temporaries <- t
|
	returnToBlock: bytePtr
		" change the location we will return to, to execute a block"
		<28 self bytePtr>
|
	copy
		^ super copy temporaries: temporaries copy

]