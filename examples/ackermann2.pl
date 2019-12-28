%% FOIL 6.0   [October 1993]
%% --------

%% Relation Ackermann

%% Relation *succ

% GM: added
% succ is built-in

ackermann(0,B,C) :-
    succ(B,C).
ackermann(A,0,C) :-
    succ(D,A),
    succ(0,E),
    ackermann(D,E,C).
ackermann(A,B,C) :-
    succ(D,A),
    succ(E,B),
    ackermann(A,E,F),
    ackermann(D,F,C).

:- ackermann(0,0,1).
:- ackermann(1,10,12).

ackermann2(0,N,K) :-
    K is N +1.
ackermann2(M,0,K) :-
    M > 0,
    M0 is M - 1,
    ackermann2(M0,1,K).
ackermann2(M,N,K) :-
    M>0,
    N>0,
    M0 is M-1,
    N0 is N-1,
    ackermann2(M,N0,F),
    ackermann2(M0,F,K).

:- ackermann2(0,0,1).
:- ackermann2(1,10,12).
:- ackermann2(3,3,K), ackermann(3,3,K2), K==K2.
%:- ackermann(3,8,K), ackermann2(3,8,K2), K==K2. % limit

