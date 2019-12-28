%% FOIL 6.0   [October 1993]
%% --------

%% diff

:- op(700,xfx,<>).
<>(A,B) :-
    A =\= B.

%% Relation *components

components([X|Xs],X,Xs).

%% Relation rows

rows(A,B,C) :-
    components(A,D,E),
    components(B,F,G),
    D<>F.
	       
:- rows([1,2,3],[2,3,1],[3,1,2]).
:- rows([1,2,3],[3,1,2],[2,3,1]).
%% :- rows([1,1,1],[2,2,2],[3,3,3]).
%% :- \+ rows([1,2,3],[1,2,3],[1,2,3].
%% :- \+ rows([1,1,1],[1,1,1],[1,1,1]).

:- use_module(library(clpfd)).

rows2([],[],[]).
rows2([R1|R1s],[R2|R2s],[R3|R3s]) :- all_different([R1,R2,R3]), rows2(R1s,R2s,R3s).

:- rows2([1,2,3],[2,3,1],[3,1,2]).
:- rows2([1,2,3],[3,1,2],[2,3,1]).
:- rows2([1,1,1],[2,2,2],[3,3,3]).
:- \+ rows2([1,2,3],[1,2,3],[1,2,3]).
:- \+ rows2([1,1,1],[1,1,1],[1,1,1]).

