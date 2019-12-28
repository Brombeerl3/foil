FOIL 6.0   [October 1993]
--------

Relation Ackermann

Relation *succ

----------
Ackermann:

State (51/9261, 511.6 bits available)


Determinate literals
	succ(A,D)
	succ(B,D) ->succ(B,E)
	succ(D,C) ->succ(F,C)

State (51/8000, 511.6 bits available)

	Save Ackermann(A,E,G) (46,920 value 146.5)
	Save Ackermann(D,A,G) (39,800 value 123.0)
	Save Ackermann(D,D,G) (39,800 value 123.0)

Best literal Ackermann(D,G,H) (10.0 bits)

State (571/12400, 493.4 bits available)


Determinate literals
	succ(G,I)
	succ(I,H) ->succ(J,H)

State (571/12400, 493.4 bits available)

	Save C=E (20,80 value 110.6)
	Save succ(B,C) (20,80 value 110.6)
	Save succ(F,E) (20,80 value 110.6)

Best literal B=F (7.5 bits)

State (380/620, 219.6 bits available)


Best literal A=0 (4.4 bits)
	Regrowing clause

State (20/441, 232.7 bits available)


Best literal succ(B,C) (5.5 bits)

Clause 0: Ackermann(A,B,C) :- A=0, succ(B,C)

State (31/9241, 339.7 bits available)


Determinate literals
	succ(A,D)
	succ(B,D) ->succ(B,E)
	succ(D,A) ->succ(F,A)
	succ(D,C) ->succ(G,C)

State (31/7600, 339.7 bits available)


Determinate literals
	Ackermann(F,B,H)
	Ackermann(F,E,H) ->Ackermann(F,E,I)
	Ackermann(F,H,C) ->Ackermann(F,J,C)

State (31/796, 339.7 bits available)


Determinate literals
	succ(H,K)
	succ(J,K) ->succ(J,L)
	succ(K,H) ->succ(M,H)
	succ(K,I) ->succ(N,I)
	succ(K,J) ->succ(O,J)

State (31/750, 339.7 bits available)

		Save clause ending with ~Ackermann(F,A,P) (cover 1, accuracy 100%)

Determinate literals
	Ackermann(F,O,P)

State (31/750, 339.7 bits available)

	Save K=L (21,39 value 153.5)
	Save M=O (21,39 value 153.5)
	Save succ(H,L) (21,39 value 153.5)
	Save succ(J,K) (21,39 value 153.5)

Best literal H=J (9.9 bits)

State (21/39, 236.8 bits available)

	Save B=M (19,19 value 155.3)
	Save C=I (19,19 value 155.3)
	Save C=K (19,19 value 155.3)
	Save E=G (19,19 value 155.3)

Best literal F=0 (5.7 bits)

Clause 1: Ackermann(A,B,C) :- succ(D,A), Ackermann(D,B,E), Ackermann(D,F,C), E=F, D=0

State (12/9222, 156.6 bits available)


Determinate literals
	succ(A,D)
	succ(B,D) ->succ(B,E)
	succ(C,D) ->succ(C,F)
	succ(D,A) ->succ(G,A)
	succ(D,C) ->succ(H,C)

State (12/7202, 156.6 bits available)


Determinate literals
	Ackermann(G,B,I)
	Ackermann(G,E,I) ->Ackermann(G,E,J)
	Ackermann(G,I,C) ->Ackermann(G,K,C)

State (12/741, 156.6 bits available)


Determinate literals
	succ(I,L)
	succ(J,L) ->succ(J,M)
	succ(K,L) ->succ(K,N)
	succ(L,I) ->succ(O,I)
	succ(L,J) ->succ(P,J)
	succ(L,K) ->succ(Q,K)

State (12/660, 156.6 bits available)

		Save clause ending with ~Ackermann(G,A,R) (cover 1, accuracy 100%)

Determinate literals
	Ackermann(G,Q,R)

State (12/660, 156.6 bits available)

	Save ~Ackermann(A,S,H) (12,235 value 63.5)
	Save ~Ackermann(A,S,Q) (12,261 value 61.7)

Best literal ~Ackermann(A,S,F) (12.1 bits)

State (12/218, 144.5 bits available)

	Save B=Q (3,18 value 21.7)
	Save C=J (3,18 value 21.7)
	Save E=K (3,18 value 21.7)
	Save F=M (3,18 value 21.7)

Best literal B=0 (6.2 bits)
	Regrowing clause

State (3/439, 48.9 bits available)


Determinate literals
	succ(A,D)
	succ(C,D) ->succ(C,E)
	succ(D,A) ->succ(F,A)
	succ(D,C) ->succ(G,C)

State (3/360, 48.9 bits available)


Determinate literals
	Ackermann(B,D,H)
	Ackermann(B,E,H) ->Ackermann(B,E,I)
	Ackermann(B,H,F) ->Ackermann(B,J,F)
	Ackermann(B,H,G) ->Ackermann(B,K,G)
	Ackermann(F,B,H) ->Ackermann(F,B,L)
	Ackermann(F,H,C) ->Ackermann(F,M,C)

State (3/28, 48.9 bits available)


Best literal succ(B,M) (9.6 bits)

Clause 2: Ackermann(A,B,C) :- B=0, succ(D,A), Ackermann(D,E,C), succ(B,E)

State (9/9219, 124.6 bits available)


Determinate literals
	succ(A,D)
	succ(B,D) ->succ(B,E)
	succ(C,D) ->succ(C,F)
	succ(D,A) ->succ(G,A)
	succ(D,B) ->succ(H,B)
	succ(D,C) ->succ(I,C)

State (9/6842, 124.6 bits available)


Determinate literals
	Ackermann(A,H,J)
	Ackermann(G,A,J) ->Ackermann(G,A,K)
	Ackermann(G,B,J) ->Ackermann(G,B,L)
	Ackermann(G,D,J) ->Ackermann(G,D,M)
	Ackermann(G,E,J) ->Ackermann(G,E,N)
	Ackermann(G,G,J) ->Ackermann(G,G,O)
	Ackermann(G,H,J) ->Ackermann(G,H,P)
	Ackermann(G,J,C) ->Ackermann(G,Q,C) Q=J (no new vars)

State (9/534, 124.6 bits available)


Best literal Ackermann(G,J,C) (11.3 bits)

Clause 3: Ackermann(A,B,C) :- succ(D,A), succ(E,B), Ackermann(A,E,F), Ackermann(D,F,C)

Delete clause
	Ackermann(A,B,C) :- succ(D,A), Ackermann(D,B,E), Ackermann(D,F,C), E=F, D=0

Ackermann(0,B,C) :- succ(B,C)
Ackermann(A,0,C) :- succ(D,A), Ackermann(D,E,C), succ(0,E)
Ackermann(A,B,C) :- succ(D,A), succ(E,B), Ackermann(A,E,F), Ackermann(D,F,C)

Time 0.5 secs
