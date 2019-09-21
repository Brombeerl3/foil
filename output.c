/******************************************************************************/
/*									      */
/*	All output routines						      */
/*									      */
/******************************************************************************/


#include  "defns.i"
#include  "extern.i"


void  PrintTuple(Tuple C, int N, TypeInfo *TypeRef, Boolean ShowPosNeg)
/*    ----------  */
{
    int		i;

    printf("\t\t");

    ForEach(i, 1, N)
    {
        if ( TypeRef ? TypeRef[i]->Continuous :
		       Variable[i]->TypeRef->Continuous )
	{
	    /*  Continuous value  */

	    if ( FP(C[i]) == MISSING_FP )
	    {
		printf("?");
	    }
	    else
	    {
		printf("%g", FP(C[i]));
	    }
	}
	else
	{
	    printf("%s", ConstName[C[i]]);
	}

	if ( i < N ) putchar(',');
    }

    if ( ShowPosNeg )
    {
	printf(": %c", (Positive(C) ? '+' : '-') );
    }

    putchar('\n');
}


    
void  PrintTuples(Tuple *TT, int N)
/*    -----------  */
{
    while ( *TT )
    {
	PrintTuple(*TT, N, Nil, true);
	TT++;
    }
}



void  PrintSpecialLiteral(Relation R, Boolean RSign, Var *A)
/*    -------------------  */
{
    Const	ThConst;
    float	Thresh;

    if ( R == EQVAR )
    {
        printf("%s%s%s", Variable[A[1]]->Name, RSign ? "=":"<>",
                         Variable[A[2]]->Name);
    }
    else
    if ( R == EQCONST )
    {
	GetParam(&A[2], &ThConst);

	printf("%s%s%s", Variable[A[1]]->Name, RSign ? "=" : "<>",
			 ConstName[ThConst]);
    }
    else
    if ( R == GTVAR )
    {
        printf("%s%s%s", Variable[A[1]]->Name, RSign ? ">" : "<=",
                         Variable[A[2]]->Name);
    }
    else
    if ( R == GTCONST )
    {
	GetParam(&A[2], &Thresh);

	if ( Thresh == MISSING_FP )
	{
	    printf("%s%s", Variable[A[1]]->Name, RSign ? ">" : "<=");
	}
	else
	{
	    printf("%s%s%g", Variable[A[1]]->Name, RSign ? ">" : "<=", Thresh);
	}
    }
}



void  PrintComposedLiteral(Relation R, Boolean RSign, Var *A)
/*    --------------------  */
{
    int i, v;

    if ( Predefined(R) )
    {
	PrintSpecialLiteral(R, RSign, A);
    }
    else
    {
	if ( ! RSign )
	{
	    putchar('~');
	}

	printf("%s", R->Name);
	ForEach(i, 1, R->Arity)
	{
	    v = A[i];
	    printf("%c%s", (i > 1 ? ',' : '('), (v ? Variable[v]->Name : "*"));
	}
	putchar(')');
    }
}



void  PrintLiteral(Literal L)
/*    ------------  */
{
    PrintComposedLiteral(L->Rel, L->Sign, L->Args);
}



void  PrintClause(Relation R, Clause C)
/*    -----------  */
{
    int		Lit;

    PrintComposedLiteral(R, true, DefaultVars);

    for ( Lit = 0 ; C[Lit] ; Lit++ )
    {
	printf("%s ", ( Lit ? "," : " :-" ));

	PrintLiteral(C[Lit]);
    }
    putchar('\n');
}



	/*  Print clause, substituting for variables equivalent to constants  */

void  PrintSimplifiedClause(Relation R, Clause C)
/*    ---------------------  */
{
    int		Lit;
    Literal	L;
    Var		V, W;
    char	**HoldVarNames;
    Const	TC;
    Boolean	Change, NeedComma;

    /*  Set up alternate names for variables equated to theory constants  */

    HoldVarNames = AllocZero(MAXVARS+1, char *);

    for ( Lit = 0 ; L = C[Lit] ; Lit++ )
    {
        if ( L->Rel == EQCONST && L->Sign )
	{
	    V = L->Args[1];
	    GetParam(&L->Args[2], &TC);

	    HoldVarNames[V]   = Variable[V]->Name;
	    Variable[V]->Name = ConstName[TC];
	}
    }

    /*  Propagate changes through variable equalities  */

    do
    {
	Change = false;

	for ( Lit = 0 ; L = C[Lit] ; Lit++ )
	{
	    if ( L->Rel == EQVAR && L->Sign )
	    {
		V = L->Args[1];
		W = L->Args[2];

		if ( HoldVarNames[V]  && ! HoldVarNames[W] )
	 	{
		    HoldVarNames[W] = Variable[W]->Name;
		    Variable[W]->Name = Variable[V]->Name;
		    Change = true;
		}
		else
		if ( HoldVarNames[W]  && ! HoldVarNames[V] )
	 	{
		    HoldVarNames[V] = Variable[V]->Name;
		    Variable[V]->Name = Variable[W]->Name;
		    Change = true;
		}
		else
		if ( ! HoldVarNames[W] )
		{
		    HoldVarNames[W] = Variable[W]->Name;
		    Variable[W]->Name = Variable[V]->Name;
		    Change = true;
		}
	    }
	}
    } while ( Change );

    PrintComposedLiteral(R, true, DefaultVars);
    printf(" :- ");
    NeedComma = false;

    for ( Lit = 0 ; (L = C[Lit]) ; Lit++ )
    {
	/*  Skip literals that are implicit in changed names  */

        if ( ( L->Rel == EQCONST ||
	       L->Rel == EQVAR && HoldVarNames[L->Args[1]] ) &&
	     L->Sign )
	{
	    continue;
	}

	if ( NeedComma )
	{
	    printf(", ");
	}

	PrintLiteral(L);
	NeedComma = true;
    }
    putchar('\n');

    ForEach(V, 1, MAXVARS)
    {
	if ( HoldVarNames[V] ) Variable[V]->Name = HoldVarNames[V];
    }

    pfree(HoldVarNames);
}



void  PrintDefinition(Relation R)
/*    ---------------  */
{
    int		Cl, SecondPass=(-1);
    Clause	C;

    putchar('\n');
    for ( Cl = 0 ; C=R->Def[Cl] ; Cl++ )
    {
	if ( Recursive(C) )
	{
	    SecondPass = Cl;
	}
	else
	{
	    PrintSimplifiedClause(R, C);
	}
    }

    ForEach(Cl, 0, SecondPass)
    {
	if ( Recursive(C = R->Def[Cl]) ) PrintSimplifiedClause(R, C);
    }

    printf("\nTime %.1f secs\n", CPUTime());
}



Boolean  Recursive(Clause C)
/*       ---------  */
{
    int Lit;

    for ( Lit = 0 ; C[Lit] ; Lit++ )
    {
	if ( C[Lit]->Rel == Target ) return true;
    }

    return false;
}
