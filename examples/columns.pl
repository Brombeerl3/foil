%% FOIL 6.0   [October 1993]
%% --------

%% diff

:- op(700,xfx,<>).
<>(A,B) :-
    A =\= B.

%% Relation *components

components([X|Xs],X,Xs).

%% Relation rows

cols(A,B,C) :-
    components(A,D,E),
    components(B,F,G),
    D<>F.
	       
:- cols([1,2,3],[2,3,1],[3,1,2]).
:- cols([1,2,3],[3,1,2],[2,3,1]).
%% :- cols([1,1,1],[2,2,2],[3,3,3]).
%% :- \+ cols([1,2,3],[1,2,3],[1,2,3].
%% :- \+ cols([1,1,1],[1,1,1],[1,1,1]).

:- use_module(library(clpfd)).

columns([],[],[]).
columns([R1|R1s],[R2|R2s],[R3|R3s]) :- all_different([R1,R2,R3]), columns(R1s,R2s,R3s).

:- columns([1,2,3],[2,3,1],[3,1,2]).
:- columns([1,2,3],[3,1,2],[2,3,1]).
:- columns([1,1,1],[2,2,2],[3,3,3]).
:- \+ columns([1,2,3],[1,2,3],[1,2,3]).
:- \+ columns([1,1,1],[1,1,1],[1,1,1]).

gencols([A1,A2,A3],[B1,B2,B3],[C1,C2,C3]) :-
    [A1,A2,A3,B1,B2,B3,C1,C2,C3] ins 1..3,
    columns([A1,A2,A3],[B1,B2,B3],[C1,C2,C3]),
    label([A1,A2,A3,B1,B2,B3,C1,C2,C3]).

writecols([A1,A2,A3],[B1,B2,B3],[C1,C2,C3]) :-
    gencols([A1,A2,A3],[B1,B2,B3],[C1,C2,C3]),
    write('['),write(A1),write(A2),write(A3),write(']'), write(','),
    write('['),write(B1),write(B2),write(B3),write(']'), write(','),
    write('['),write(C1),write(C2),write(C3),write(']'), nl,
    fail.

:- gencols(As,Bs,Cs), As=[1,1,1], Bs=[2,2,2], Cs=[3,3,3].
