/******************************************************************************/
/*									      */
/*	Stuff for pruning clauses and definitions.  Clauses are first	      */
/*	`quickpruned' to remove determinate literals that introduce unused    */
/*	variables; if this causes the definition to become less accurate,     */
/*	these are restored.  Then literals are removed one at a time to	      */
/*	see whether they contribute anything.  A similar process is	      */
/*	followed when pruning definitions: clauses are removed one at a	      */
/*	time to see whether the remaining clauses suffice		      */
/*									      */
/******************************************************************************/


#include  "defns.i"
#include  "extern.i"

extern Boolean RecordArgOrders;	/* for interpret.c */

#define  MarkLiteral(L,X)	{ (L)->Args[0]=X;\
				  if ((L)->ArgOrders) (L)->ArgOrders[0]=X;}


	/*  See whether literals can be dropped from a clause  */

void  PruneNewClause()
/*    --------------  */
{
    int		Errs, i, j;
    Boolean	*Used, Changed=false;
    Literal	L;

    Errs = Current.NOrigTot - Current.NOrigPos;

    Used = Alloc(MAXVARS+1, Boolean);

    Verbose(2)
    {
	printf("\nInitial clause (%d errs): ", Errs);
	PrintClause(Target, NewClause);
    }

    /*  Save arg orderings for recursive literals  */

    ForEach(i, 0, NLit-1)
    {
	L = NewClause[i];
	if ( L->Rel == Target )
	{
	    L->SaveArgOrders = Alloc(Target->Arity+1, Ordering);
	    memcpy(L->SaveArgOrders, L->ArgOrders, Target->Arity+1);
	}
    }

    if ( QuickPrune(NewClause, Target->Arity, Used) )
    {
	if ( SatisfactoryNewClause(Errs) )
	{
	    Verbose(3) printf("\tAccepting quickpruned clause\n");
	    Changed = true;
	}
	else
	{
	    /*  Mark all literals as active again  */

	    ForEach(i, 0, NLit-1)
	    {
		MarkLiteral(NewClause[i], 0);
	    }
	}
    }

    Changed |= RedundantLiterals(Errs);

    /*  Remove unnecessary literals from NewClause  */

    if ( Changed )
    {
	for ( i = 0 ; i < NLit ; )
	{
	    L = NewClause[i];
	    if ( L->Args[0] )
	    {
		FreeLiteral(L);
		NLit--;
		ForEach(j, i, NLit-1)
		{
		    NewClause[j] = NewClause[j+1];
		}
	    }
	    else
	    {
	       i++;
	    }
	}
	NewClause[NLit] = Nil;

	RenameVariables();

	/*  Need to recompute partial orders and clause coverage  */

	RecoverState(NewClause);
	CheckOriginalCaseCover();
    }
    else
    {
	/*  Restore arg orders  */

	ForEach(i, 0, NLit-1)
	{
	    L = NewClause[i];
	    if ( L->Rel == Target )
	    {
		memcpy(L->ArgOrders,L->SaveArgOrders,
                       (Target->Arity+1)*sizeof(Ordering));
                pfree(L->SaveArgOrders);
	    }
	}
    }

    pfree(Used);
}



	/*  Remove determinate literals that introduce useless new variables
	    (that are not used by any subsequent literal)  */

Boolean  QuickPrune(Clause C, Var MaxBound, Boolean *Used)
/*       ----------  */
{
    Var		V, NewMaxBound;
    Literal	L;
    Boolean	SomeUsed=true, Changed=false;

    if ( (L = C[0]) == Nil )
    {
	ForEach(V, 1, MaxBound) Used[V] = false;
	return false;
    }

    L->Args[0] = 0;

    NewMaxBound = MaxBound;
    ForEach(V, 1, L->Rel->Arity)
    {
	if ( L->Args[V] > NewMaxBound ) NewMaxBound = L->Args[V];
    }

    Changed = QuickPrune(C+1, NewMaxBound, Used);

    if ( L->Sign == 2 )
    {
	SomeUsed = false;
	ForEach(V, MaxBound+1, NewMaxBound)
	{
	    SomeUsed = Used[V];
	    if ( SomeUsed ) break;
	}
    }

    if ( ! SomeUsed && C[1] )
    {
	/*  Mark this literal as inactive  */

	MarkLiteral(L, 1);

	Verbose(3)
	{
	    printf("\tQuickPrune literal ");
	    PrintLiteral(L);
	    putchar('\n');
	}

	Changed = true;
    }
    else
    ForEach(V, 1, L->Rel->Arity)
    {
	Used[L->Args[V]] = true;
    }

    return Changed;
}



Boolean  SatisfactoryNewClause(int Errs)
/*       ---------------------  */
{
    Boolean	RecursiveLits=false, ProForma;
    Literal	L;
    int		i, ErrsNow=0;

    /*  Prepare for redetermining argument orders  */

    ForEach(i, 0, NLit-1)
    {
	if ( (L = NewClause[i])->Args[0] ) continue;

	if ( L->Rel == Target )
	{
	    RecursiveLits = true;
	    memset(L->ArgOrders, 0, Target->Arity+1);
	}
    }

    /*  Scan case by case, looking for too many covered neg tuples  */

    RecordArgOrders = RecursiveLits;

    ForEach(i, StartDef.NPos, StartDef.NTot-1)
    {
	InitialiseValues(StartDef.Tuples[i], Target->Arity);

	if ( CheckRHS(NewClause) && ++ErrsNow > Errs ||
	     RecursiveLits && ! RecordArgOrders )
	{
	    RecordArgOrders = false;
	    return false;
	}
    }

    /*  If satisfactory and recursive literals, must also check pos
	tuples covered to complete arg order information  */

    for ( i = 0 ; i < StartDef.NPos && RecordArgOrders ; i++ )
    {
	InitialiseValues(StartDef.Tuples[i], Target->Arity);

	ProForma = CheckRHS(NewClause);
    }

    /*  Now check that recursion still well-behaved  */

    return ( RecursiveLits ? RecordArgOrders && RecursiveCallOK(Nil) : true );
}


    /*  Rename variables in NewClause  */

void  RenameVariables()
/*    ---------------  */
{
    Var		*NewVar, Next, SaveNext, V;
    int		l, i;
    Literal	L;

    NewVar = AllocZero(MAXVARS+1, Var);
    Next = Target->Arity+1;

    ForEach(l, 0, NLit-1)
    {
	if ( (L=NewClause[l])->Args[0] ) continue;

	SaveNext = Next;
	ForEach(i, 1, L->Rel->Arity)
	{
	    V = L->Args[i];

	    if ( V > Target->Arity )
	    {
		if ( ! NewVar[V] ) NewVar[V] = Next++;

		L->Args[i] = NewVar[V];
	    }
	}

	/*  Restore next variable after negative literal  */

	if ( ! L->Sign )
	{
	    Next = SaveNext;

	    ForEach(V, 1, MAXVARS)
	    if ( NewVar[V] >= Next ) NewVar[V] = 0;
	}
    }

    pfree(NewVar);
}



	/*  Omit the first unnecessary literal.
	    This version prunes from the end of the clause; if a literal
	    can't be dropped when it is examined, it will always be
	    needed, since dropping earlier literals can only increase
	    the number of minus tuples getting through to this literal  */


Boolean  RedundantLiterals(int ErrsNow)
/*       -----------------  */
{
    int		i;
    Boolean	Changed=false;

    /*  Check for the latest literal, omitting which would not increase
	the number of errors.  Do not try to remove the very last literal
	unless it is determinate  */

    for ( i = (NewClause[NLit-1]->Sign == 2 ? NLit-1 : NLit-2) ; i >= 0 ; i-- )
    {
	/*  Can't omit a literal that is needed to bind a variable appearing in
	    a later negated literal  */

        if ( NewClause[i]->Args[0] ||
	     ( NewClause[i]->Sign && EssentialBinding(i) ) )
	{
	    continue;
	}

	MarkLiteral(NewClause[i], 1);

	if ( SatisfactoryNewClause(ErrsNow) )
	{
	    Verbose(3)
	    {
		printf("\t\t");
		PrintLiteral(NewClause[i]);
		printf(" removed\n");
	    }
	    Changed = true;
	}
	else
	{
	    Verbose(3)
	    {
		printf("\t\t");
		PrintLiteral(NewClause[i]);
		printf(" essential\n");
	    }
	    MarkLiteral(NewClause[i], 0);
	}
    }

    return Changed;
}



	/*  Can't omit a literal that is needed to bind a variable appearing in
	    a later negated literal relation, or whose omission would leave a
	    later literal containing all new variables  */

#define  NONE	-1
#define  MANY	1000000

Boolean  EssentialBinding(int LitNo)
/*       ----------------  */
{
    int		i, j, NeedBound, *UniqueBinding;
    Literal	L;
    Var		V, HiVar;

    /*  UniqueBinding[V] = NONE (if V not yet bound)
			 = i    (if V bound only by ith literal)
			 = MANY (if V bound by more than one literal)  */

    UniqueBinding = Alloc(MAXVARS+1, int);
    ForEach(V, 1, MAXVARS)
    {
	UniqueBinding[V] = ( V <= Target->Arity ? MANY : NONE );
    }

    HiVar = Target->Arity;
    ForEach(i, 0, NLit-1)
    {
	if ( (L = NewClause[i])->Args[0] ) continue;

	if ( i > LitNo )
	{
	    /*  Check that minimun number of arguments are bound  */

	    NeedBound = ( Predefined(L->Rel) || ! L->Sign ? L->Rel->Arity : 1 );

	    ForEach(j, 1, L->Rel->Arity)
	    {
		V = L->Args[j];

		if ( ! L->Sign && V > HiVar ||
		     UniqueBinding[V] != NONE && UniqueBinding[V] != LitNo )
		{
		    NeedBound--;
		}
	    }

	    if ( NeedBound > 0 )
	    {
		Verbose(3)
		{
		    printf("\t\t");
		    PrintLiteral(NewClause[LitNo]);
		    printf(" needed for binding ");
		    PrintLiteral(NewClause[i]);
		    putchar('\n');
		}

		pfree(UniqueBinding);
		return true;
	    }
	}

	if ( L->Sign )
	{
	    /*  Update binding records for new variables  */

	    ForEach(j, 1, L->Rel->Arity)
	    {
		V = L->Args[j];
		if ( V > HiVar ) HiVar = V;

		if ( UniqueBinding[V] == NONE )
		{
		    UniqueBinding[V] = i;
		}
		else
		if ( UniqueBinding[V] != i )
		{
		    UniqueBinding[V] = MANY;
		}
	    }
	}
    }

    pfree(UniqueBinding);
    return false;
}




	/*  See whether some clauses can be dispensed with  */

void  SiftClauses()
/*    -----------  */
{
    int		i, j, Covers, Last, Retain=0, Remove=0;
    Boolean	*Need, *Delete;

    if ( ! NCl ) return;

    Delete = AllocZero(NCl, Boolean);
    Need   = AllocZero(NCl, Boolean);

    Current.MaxVar = HighestVarInDefinition(Target);

    while ( Retain+Remove < NCl )
    {
	/*  See if a clause uniquely covers a pos tuple  */

	for ( i = 0 ; i < StartDef.NPos && Retain < NCl ; i++ )
	{
	    Covers = 0;
	    for ( j = 0 ; j < NCl && Covers <= 1 && Retain < NCl ; j++ )
	    {
		if ( Delete[j] ) continue;

		InitialiseValues(StartDef.Tuples[i], Target->Arity);

		if ( CheckRHS(Target->Def[j]) )
		{
		    Covers++;
		    Last = j;
		}
	    }

	    if ( Covers == 1 && ! Need[Last] )
	    {
		Verbose(3)
		{
		    printf("\tClause %d needed for ", Last);
		    PrintTuple(StartDef.Tuples[i], Target->Arity,
			       Target->TypeRef, false);
		}

		Need[Last] = true;
		Retain++;
	    }
	}

	if ( Retain == NCl ) break;

	/*  Remove the latest unneeded clause  */

	Last = -1;
	ForEach(i, 0, NCl-1)
	{
	    if ( ! Need[i] && ! Delete[i] ) Last = i;
	}

	if ( Last == -1 ) break;

	Delete[Last] = true;
	Remove++;
    }


    if ( Retain < NCl )
    {
	Verbose(1) printf("\nDelete clause%s\n", Plural(NCl - Retain));

	Retain = 0;

	ForEach(i, 0, NCl-1)
	{
	    if ( Delete[i] )
	    {
		Verbose(1)
		{
		    printf("\t");
		    PrintClause(Target, Target->Def[i]);
		    FreeClause(Target->Def[i]);
		}
	    }
	    else
	    {
		Target->Def[Retain++] = Target->Def[i];
	    }
	}
	Target->Def[Retain] = Nil;
    }

    pfree(Delete);
    pfree(Need);
}



    /*  Find highest variable in any clause  */

Var  HighestVarInDefinition(Relation R)
/*   ----------------------  */
{
    Var		V, HiV;
    Literal	L;
    Clause	C;
    int		i;

    HiV = R->Arity;

    for ( i = 0 ; R->Def[i] ; i++ )
    {
	for ( C = R->Def[i] ; L = *C ; C++ )
	{
	    if ( L->Sign )
	    {
		ForEach(V, 1, L->Rel->Arity)
		{
		    if ( L->Args[V] > HiV ) HiV = L->Args[V];
		}
	    }
	}
    }

    return HiV;
}
