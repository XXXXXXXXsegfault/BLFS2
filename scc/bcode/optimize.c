struct exp_list
{
	void *ptr;
	struct exp_list *next;
} *exp_list;
struct exp
{
	struct ins *ins;
	struct exp *next;
};
struct exp_set
{
	unsigned long int key;
	struct exp *exp;
	struct exp *exp_end;
	struct exp_set *next;
};
struct exp_set *exp_set[967],*exp_set2[967],*exp_set3[967],*exp_set4[967],*exp_set5[967],*exp_set6[967],*exp_set7[967];
struct exp *global_exp;
#define DF_BMSIZE 64
#define DF_QWORDS 1
void exp_set_add(struct exp_set **set,unsigned long int key,struct ins *ins)
{
	struct exp_set *node;
	struct exp *exp;
	struct exp_list *l;
	int hash;
	hash=key%967;
	node=set[hash];
	while(node&&node->key!=key)
	{
		node=node->next;
	}
	if(!node)
	{
		node=xmalloc(sizeof(*node));
		node->key=key;
		node->exp=0;
		node->exp_end=0;
		node->next=set[hash];
		set[hash]=node;
		l=xmalloc(sizeof(*l));
		l->ptr=node;
		l->next=exp_list;
		exp_list=l;
	}
	exp=xmalloc(sizeof(*exp));
	exp->ins=ins;
	exp->next=0;
	if(node->exp)
	{
		node->exp_end->next=exp;
	}
	else
	{
		node->exp=exp;
	}
	node->exp_end=exp;
	l=xmalloc(sizeof(*l));
	l->ptr=exp;
	l->next=exp_list;
	exp_list=l;
}
struct exp *exp_set_find(struct exp_set **set,unsigned long key)
{
	struct exp_set *node;
	int hash;
	hash=key%967;
	node=set[hash];
	while(node&&node->key!=key)
	{
		node=node->next;
	}
	if(node)
	{
		return node->exp;
	}
	return 0;
}
void exp_set_release(void)
{
	struct exp_list *node;
	while(node=exp_list)
	{
		exp_list=node->next;
		free(node->ptr);
		free(node);
	}
}
void global_exp_add(struct ins *ins)
{
	struct exp *exp;
	struct exp_list *l;
	exp=xmalloc(sizeof(*exp));
	exp->ins=ins;
	exp->next=global_exp;
	global_exp=exp;
	l=xmalloc(sizeof(*l));
	l->ptr=exp;
	l->next=exp_list;
	exp_list=l;
}

int var_num_match(struct ins *ins,struct ins *exp)
{
	if(ins->var_num[0]==0)
	{
		return 0;
	}
	if(ins->var_num[0]==exp->var_num[1])
	{
		return 1;
	}
	if(ins->var_num[0]==exp->var_num[2])
	{
		return 1;
	}
	return 0;
}
int expcmp(struct ins *exp1,struct ins *exp2)
{
	struct id_tab *id;
	if(exp1->count_args==0)
	{
		return 0;
	}
	if(exp2->count_args==0)
	{
		return 0;
	}
	if(strcmp(exp1->args[0],exp2->args[0]))
	{
		return 0;
	}
	if(exp1->var_num[0]==0)
	{
		return 0;
	}
	if(exp2->var_num[0]==0)
	{
		return 0;
	}
	if(exp1->is_const[0]!=exp2->is_const[0])
	{
		return 0;
	}
	if(exp1->is_const[1]!=exp2->is_const[1])
	{
		return 0;
	}
	if(exp1->var_num[1]!=exp2->var_num[1])
	{
		return 0;
	}
	if(exp1->var_num[2]!=exp2->var_num[2])
	{
		return 0;
	}
	if(exp1->count_args>=3)
	{
		if(exp1->var_num[1]==0)
		{
			if(exp1->is_const[0]!=1)
			{
				return 0;
			}
			if(exp1->const_val[0]!=exp2->const_val[0])
			{
				return 0;
			}
		}
	}
	if(exp1->count_args>=4)
	{
		if(exp1->var_num[2]==0)
		{
			if(exp1->is_const[1]!=1)
			{
				return 0;
			}
			if(exp1->const_val[1]!=exp2->const_val[1])
			{
				return 0;
			}
		}
	}
	return 1;
}
struct ins *init_gen_kill(struct ins *estart)
{
	struct ins *ins;
	struct exp *exp;
	unsigned int x;
	unsigned long int a;
	a=1;
	x=0;
	while(x!=DF_BMSIZE&&estart&&estart!=fend)
	{
		if(estart->var_num[0]&&(estart->var_num[1]||estart->is_const[0])&&!var_num_match(estart,estart))
		{
			if((estart->op==1||estart->op==12||estart->op==11||estart->op==6||(estart->op==2||estart->op==13||estart->op==14||estart->op==15||estart->op==17)&&(estart->var_num[2]||estart->is_const[1])))
			{
				estart->valuse[x>>6]|=a<<(x&63);
				if(estart->var_num[1])
				{
					exp=exp_set_find(exp_set,estart->var_num[1]);
					while(exp)
					{
						ins=exp->ins;
						ins->valdef[x>>6]|=a<<(x&63);
						if(ins->op==11)
						{
							if(var_num_match(estart,ins))
							{
								estart->valuse[x>>6]&=~(a<<(x&63));
								break;
							}
						}
						exp=exp->next;
					}
				}
				if(estart->var_num[2])
				{
					exp=exp_set_find(exp_set,estart->var_num[2]);
					while(exp)
					{
						ins=exp->ins;
						ins->valdef[x>>6]|=a<<(x&63);
						exp=exp->next;
					}
				}
				exp=exp_set_find(exp_set,estart->var_num[0]);
				while(exp)
				{
					ins=exp->ins;
					if(expcmp(ins,estart))
					{
						ins->valuse[x>>6]|=a<<(x&63);
					}
					else
					{
						ins->valdef[x>>6]|=a<<(x&63);
					}
					exp=exp->next;
				}
				if(estart->is_global[0]||estart->is_global[1]||estart->is_global[2]||estart->op==6||estart->op==15)
				{
					exp=global_exp;
					while(exp)
					{
						ins=exp->ins;
						ins->valdef[x>>6]|=a<<(x&63);
						exp=exp->next;
					}
				}
				if(estart->valuse[x>>6]&a<<(x&63))
				{
					estart->mark3=x;
				}
				++x;
			}
		}
		estart=estart->next;
	}
	return estart;
}
void calculate_available_exp(struct ins *start,struct ins *end)
{
	struct ins *ins;
	int s,x;
	unsigned long int old_out;
	ins=fstart;
	while(ins&&ins!=fend)
	{
		memset(ins->valout,0xff,sizeof(ins->valout));
		ins=ins->next;
	}
	do
	{
		s=0;
		ins=fstart;
		while(ins&&ins!=fend)
		{
			memset(ins->valin,0,sizeof(ins->valin));
			ins->mark=1;
			ins=ins->next;
		}
		ins=fstart;
		while(ins&&ins!=fend)
		{
			if(ins->next&&ins->next!=fend&&ins->op!=8&&ins->op!=9&&ins->op!=10)
			{
				if(ins->next->mark)
				{
					memset(ins->next->valin,0xff,sizeof(ins->valin));
					ins->next->mark=0;
				}
				x=0;
				while(x<DF_QWORDS)
				{
					ins->next->valin[x]&=ins->valout[x];
					++x;
				}
			}
			if(ins->branch)
			{
				if(ins->branch->mark)
				{
					memset(ins->branch->valin,0xff,sizeof(ins->valin));
					ins->branch->mark=0;
				}
				x=0;
				while(x<DF_QWORDS)
				{
					ins->branch->valin[x]&=ins->valout[x];
					++x;
				}
			}
			ins=ins->next;
		}
		ins=fstart;
		while(ins&&ins!=fend)
		{
			x=0;
			while(x<DF_QWORDS)
			{
				old_out=ins->valout[x];
				ins->valout[x]=ins->valuse[x]|ins->valin[x]&~ins->valdef[x];
				if(old_out!=ins->valout[x])
				{
					s=1;
				}
				++x;
			}
			ins=ins->next;
		}
	}
	while(s);
}
int delete_common_exp(int delete_add_sub)
{
	struct ins *ins,*start,*p,*p1;
	struct exp *exp;
	unsigned int s,x,s1;
	unsigned long int a;
	a=1;
	ins=fstart;
	s=0;
	while(ins&&ins!=fend)
	{
		p=fstart;
		while(p&&p!=fend)
		{
			memset(p->valdef,0,sizeof(p->valdef));
			memset(p->valuse,0,sizeof(p->valuse));
			p->mark3=0xffff;
			p->mark2=0;
			p=p->next;
		}
		start=ins;
		ins=init_gen_kill(ins);
		calculate_available_exp(start,ins);
		p=fstart;

		while(p&&p!=fend)
		{
			if((p->op==1||p->op==12||p->op==2||p->op==6||p->op==11||p->op==15)||(p->op==13||p->op==14||p->op==17))
			{
				if(p->var_num[1])
				{
					exp=exp_set_find(exp_set2,p->var_num[1]+p->var_num[2]*1021);
					while(exp)
					{
						p1=exp->ins;
						if(!p1->mark2&&expcmp(p1,p)&&p!=p1&&(x=p1->mark3)<DF_BMSIZE)
						{
							if(p->valin[x>>6]&a<<(x&63))
							{
								if(p1->op==12&&!p1->is_global[1])
								{
									if(delete_add_sub)
									{
										p->args[0]="mov";
										p->op=12;
										p->count_args=3;
										if(p->var_num[1]!=p1->var_num[1])
										{
											s=1;
										}
										p->args[2]=p1->args[2];
										p->var_num[1]=p1->var_num[1];
										p->is_global[1]=0;
										if(p->args[3])
										{
											p->args[3]=0;
										}
										p->mark2=1;
										p->var_num[2]=0;
										p->is_const[0]=0;
										p->is_const[1]=0;
									}
									break;
								}
								else if(!p1->is_global[0]&&p1->var_num[1])
								{
									p->args[0]="mov";
									p->op=12;
									p->count_args=3;
									if(p->var_num[1]!=p1->var_num[0])
									{
										s=1;
									}
									p->args[2]=p1->args[1];
									p->var_num[1]=p1->var_num[0];
									p->is_global[1]=0;
									if(p->args[3])
									{
										p->args[3]=0;
									}
									p->mark2=1;
									p->var_num[2]=0;
									p->is_const[0]=0;
									p->is_const[1]=0;
									break;
								}
							}
						}
						exp=exp->next;
					}
				}
			}
			p=p->next;
		}
	}
	return s;
}
int delete_common_exp2(void)
{
	struct ins *ins,*start,*p,*p1;
	struct exp *exp;
	unsigned int s,x,s1;
	unsigned long int a;
	a=1;
	ins=fstart;
	s=0;
	while(ins&&ins!=fend)
	{
		p=fstart;
		while(p&&p!=fend)
		{
			memset(p->valdef,0,sizeof(p->valdef));
			memset(p->valuse,0,sizeof(p->valuse));
			p->mark3=0xffff;
			p->mark2=0;
			p=p->next;
		}
		start=ins;
		ins=init_gen_kill(ins);
		calculate_available_exp(start,ins);
		p=fstart;

		while(p&&p!=fend)
		{
			if((p->op==1||p->op==12||p->op==2||p->op==13||p->op==14||p->op==7||p->op==17))
			{
				if(p->var_num[1])
				{
					exp=exp_set_find(exp_set3,p->var_num[1]);
					while(exp)
					{
						p1=exp->ins;
						if(p!=p1&&!p1->mark2&&(x=p1->mark3)<DF_BMSIZE)
						{
							if(p->valin[x>>6]&a<<(x&63))
							{
								if(p1->var_num[0]!=p1->var_num[1]&&!p1->is_global[1])
								{
									p->mark2=1;
									p->var_num[1]=p1->var_num[1];
									p->args[2]=p1->args[2];
									p->is_global[1]=0;
									s=1;
									break;
								}
							}
						}
						exp=exp->next;
					}
				}
			}
			if((p->op==1||p->op==12||p->op==2||p->op==13||p->op==14||p->op==7||p->op==17))
			{
				if(p->var_num[2])
				{
					exp=exp_set_find(exp_set3,p->var_num[2]);
					while(exp)
					{
						p1=exp->ins;
						if(p!=p1&&!p1->mark2&&(x=p1->mark3)<DF_BMSIZE)
						{
							if(p->valin[x>>6]&a<<(x&63))
							{
								if(p1->var_num[0]!=p1->var_num[1]&&!p1->is_global[1])
								{
									p->mark2=1;
									p->var_num[2]=p1->var_num[1];
									p->args[3]=p1->args[2];
									p->is_global[2]=0;
									s=1;
									break;
								}
							}
						}
						exp=exp->next;
					}
				}
			}
			p=p->next;
		}
	}
	return s;
}
int delete_common_exp3(void)
{
	struct ins *ins,*start,*p,*p1;
	struct exp *exp;
	unsigned int s,x,s1;
	unsigned long int a;
	a=1;
	ins=fstart;
	s=0;
	while(ins&&ins!=fend)
	{
		p=fstart;
		while(p&&p!=fend)
		{
			memset(p->valdef,0,sizeof(p->valdef));
			memset(p->valuse,0,sizeof(p->valuse));
			p->mark3=0xffff;
			p->mark2=0;
			p=p->next;
		}
		start=ins;
		ins=init_gen_kill(ins);
		calculate_available_exp(start,ins);
		p=fstart;

		while(p&&p!=fend)
		{
			//ldo
			if(p->op==6)
			{
				if(p->var_num[1])
				{
					exp=exp_set_find(exp_set4,p->var_num[1]);
					while(exp)
					{
						p1=exp->ins;
						if(p!=p1&&!p1->mark2&&(x=p1->mark3)<DF_BMSIZE&&p1->var_num[1])
						{
							if(p->valin[x>>6]&a<<(x&63)&&p1->op==13)
							{
								p->mark2=1;
								p->count_args=4;
								p->var_num[1]=p1->var_num[1];
								p->var_num[2]=p1->var_num[2];
								p->args[2]=p1->args[2];
								p->args[3]=p1->args[3];
								p->is_global[1]=p1->is_global[1];
								p->is_global[2]=p1->is_global[2];
								p->is_const[0]=p1->is_const[0];
								p->is_const[1]=p1->is_const[1];
								p->const_val[0]=p1->const_val[0];
								p->const_val[1]=p1->const_val[1];
								p->op=15;
								s=1;
								break;
							}
						}
						exp=exp->next;
					}
				}
			}
			//sto
			if(p->op==5)
			{
				if(p->var_num[0])
				{
					exp=exp_set_find(exp_set4,p->var_num[0]);
					while(exp)
					{
						p1=exp->ins;
						if(p!=p1&&!p1->mark2&&(x=p1->mark3)<DF_BMSIZE&&p1->var_num[1])
						{
							if(p->valin[x>>6]&a<<(x&63)&&p1->op==13)
							{
								p->args[1]=p->args[2];
								p->var_num[0]=p->var_num[1];
								p->is_global[0]=p->is_global[1];
								p->count_args=4;
								p->mark2=1;
								p->var_num[1]=p1->var_num[1];
								p->var_num[2]=p1->var_num[2];
								p->args[2]=p1->args[2];
								p->args[3]=p1->args[3];
								p->is_global[1]=p1->is_global[1];
								p->is_global[2]=p1->is_global[2];
								p->is_const[0]=p1->is_const[0];
								p->is_const[1]=p1->is_const[1];
								p->const_val[0]=p1->const_val[0];
								p->const_val[1]=p1->const_val[1];
								p->op=16;
								s=1;
								break;
							}
						}
						exp=exp->next;
					}
				}
			}
			p=p->next;
		}
	}
	return s;
}
int delete_common_exp4(void)
{
	struct ins *ins,*start,*p,*p1,*p2;
	struct exp *exp;
	unsigned int s,x,s1;
	unsigned long int a;
	a=1;
	ins=fstart;
	s=0;
	while(ins&&ins!=fend)
	{
		p=fstart;
		while(p&&p!=fend)
		{
			memset(p->valdef,0,sizeof(p->valdef));
			memset(p->valuse,0,sizeof(p->valuse));
			p->mark3=0xffff;
			p->mark2=0;
			p=p->next;
		}
		start=ins;
		ins=init_gen_kill(ins);
		calculate_available_exp(start,ins);
		p=fstart;

		while(p&&p!=fend)
		{
			if(p->op==13)
			{
				if(p->var_num[1]&&p->is_const[1])
				{
					exp=exp_set_find(exp_set5,p->var_num[1]);
					while(exp)
					{
						p1=exp->ins;
						if(p!=p1&&!p1->mark2&&(x=p1->mark3)<DF_BMSIZE)
						{
							if(p->valin[x>>6]&a<<(x&63))
							{
								if(p1->var_num[0]!=p1->var_num[1])
								{
									p->mark2=1;
									p->var_num[1]=p1->var_num[1];
									p->is_global[1]=p1->is_global[1];
									p->args[2]=p1->args[2];
									p->const_val[1]+=p1->const_val[1];
									p->args[3]=str_i_app(0,p->const_val[1]);
									s=1;
									break;
								}
							}
						}
						exp=exp->next;
					}
					exp=exp_set_find(exp_set6,p->var_num[1]);
					while(exp)
					{
						p1=exp->ins;
						if(p!=p1&&!p1->mark2&&(x=p1->mark3)<DF_BMSIZE)
						{
							if(p->valin[x>>6]&a<<(x&63))
							{
								if(p1->var_num[0]!=p1->var_num[1])
								{
									p->mark2=1;
									p->var_num[1]=p1->var_num[1];
									p->is_global[1]=p1->is_global[1];
									p->args[2]=p1->args[2];
									p->const_val[1]-=p1->const_val[1];
									p->args[3]=str_i_app(0,p->const_val[1]);
									s=1;
									break;
								}
							}
						}
						exp=exp->next;
					}
				}
			}
			if(p->op==17)
			{
				p2=0;
				if(p->var_num[1]&&p->is_const[1])
				{
					exp=exp_set_find(exp_set7,p->const_val[1]);
					while(exp)
					{
						p1=exp->ins;
						if(p!=p1&&!p1->mark2&&(x=p1->mark3)<DF_BMSIZE)
						{
							if(p->valin[x>>6]&a<<(x&63))
							{
								if(p1->var_num[0]!=p1->var_num[1])
								{
									p2=p1;
									break;
								}
							}
						}
						exp=exp->next;
					}
				}
				if(p2)
				{
					exp=exp_set_find(exp_set5,p->var_num[1]);
					while(exp)
					{
						p1=exp->ins;
						if(p!=p1&&!p1->mark2&&(x=p1->mark3)<DF_BMSIZE&&p1->var_num[1]==p2->var_num[1])
						{
							if(p->valin[x>>6]&a<<(x&63))
							{
								if(p1->var_num[0]!=p1->var_num[1])
								{
									p->op=13;
									p->args[0]="add";
									p->args[2]=p2->args[1];
									p->var_num[1]=p2->var_num[0];
									p->is_global[1]=p2->is_global[0];
									p->const_val[1]*=p1->const_val[1];
									p->args[3]=str_i_app(0,p->const_val[1]);
									p->mark2=1;
									s=1;
									break;
								}
							}
						}
						exp=exp->next;
					}
					exp=exp_set_find(exp_set6,p->var_num[1]);
					while(exp)
					{
						p1=exp->ins;
						if(p!=p1&&!p1->mark2&&(x=p1->mark3)<DF_BMSIZE&&p1->var_num[1]==p2->var_num[1])
						{
							if(p->valin[x>>6]&a<<(x&63))
							{
								if(p1->var_num[0]!=p1->var_num[1])
								{
									p->op=14;
									p->args[0]="sub";
									p->args[2]=p2->args[1];
									p->var_num[1]=p2->var_num[0];
									p->is_global[1]=p2->is_global[0];
									p->const_val[1]*=p1->const_val[1];
									p->args[3]=str_i_app(0,p->const_val[1]);
									p->mark2=1;
									s=1;
									break;
								}
							}
						}
						exp=exp->next;
					}
				}
			}
			p=p->next;
		}
	}
	return s;
}
void exp_set_init(void)
{
	struct exp_set *node;
	struct ins *ins;
	ins=fstart;
	while(ins&&ins!=fend)
	{
		if(ins->op==1||ins->op==2||ins->op==4||ins->op==6||ins->op==11||ins->op==12||ins->op==13||ins->op==14||ins->op==15||ins->op==17)
		{
			if(ins->var_num[0]&&ins->var_num[1])
			{
				exp_set_add(exp_set,ins->var_num[0],ins);
				exp_set_add(exp_set2,ins->var_num[1]+ins->var_num[2]*1021,ins);
				if(ins->op==12)
				{
					exp_set_add(exp_set3,ins->var_num[0],ins);
				}
			}
		}
		if(ins->op==4||ins->op==5||ins->op==16)
		{
			global_exp_add(ins);
		}
		if(ins->var_num[0]&&ins->var_num[1])
		{
			if(ins->op==13)
			{
				exp_set_add(exp_set4,ins->var_num[0],ins);
				if(ins->is_const[1])
				{
					exp_set_add(exp_set5,ins->var_num[0],ins);
				}
			}
			if(ins->op==14)
			{
				if(ins->is_const[1])
				{
					exp_set_add(exp_set6,ins->var_num[0],ins);
				}
			}
			if(ins->op==17)
			{
				if(ins->is_const[1])
				{
					exp_set_add(exp_set7,ins->const_val[1],ins);
				}
			}
		}
		ins=ins->next;
	}
}
int delete_useless_branch(void)
{
	struct ins *ins,*ins1,*ins2;
	int s;
	s=0;
	ins=fstart;
	while(ins&&ins!=fend)
	{
		if(ins->branch)
		{
			ins1=next_op(ins->next);
			ins2=next_op(ins->branch);
			if(ins1==ins2)
			{
				ins->count_args=0;
				s=1;
			}
			else if(ins2&&ins2->op==8)
			{
				ins->branch=ins2->branch;
				if(ins->op==7&&ins->count_args>=4)
				{
					ins->args[3]=ins2->args[1];
				}
				else if(ins->op==8&&ins->count_args>=2)
				{
					ins->args[1]=ins2->args[1];
				}
				s=1;
			}
		}
		ins=ins->next;
	}
	return s;
}
void init_def_use2(void)
{
	struct ins *ins;
	ins=fstart;
	while(ins&&ins!=fend)
	{
		ins->valdef[0]=0;
		ins->valuse[0]=0;
		ins=ins->next;
	}
}
int do_delete_useless_exp(void)
{
	struct ins *ins;
	int s;
	ins=fstart;
	s=0;
	while(ins&&ins!=fend)
	{
		if(ins->count_args&&ins->op!=4)
		{
			if(ins->valdef[0]&~ins->valout[0])
			{
				ins->op=0;
				ins->count_args=0;
				s=1;
			}
		}
		ins=ins->next;
	}
	return s;
}
int delete_useless_exp(void)
{
	struct ins *ins,*start;
	int is_global,is_mem;
	int s,x;
	s=0;
	x=0;
	ins=fstart;
	while(ins&&ins!=fend)
	{
		if(ins->count_args>=3)
		{
			is_global=0;
			if(!strcmp(ins->args[0],"global")||!strcmp(ins->args[0],"local"))
			{
				if(!strcmp(ins->args[0],"global"))
				{
					is_global=1;
				}
				if(!strcmp(ins->args[1],"mem")&&ins->count_args>=4)
				{
					is_mem=1;
				}
				else
				{
					is_mem=0;
				}
				if(is_mem)
				{
					id_list_add(ins->args[3]);
					++x;
				}
				else if(!is_global)
				{
					id_list_add(ins->args[2]);
					++x;
				}
				if(x==32)
				{
					x=0;
					init_def_use2();
					init_def_use();
					calculate_df();
					s|=do_delete_useless_exp();
					id_list_release();
				}
			}
		}
		ins=ins->next;
	}
	if(x)
	{
		init_def_use2();
		init_def_use();
		calculate_df();
		s|=do_delete_useless_exp();
		id_list_release();
	}
	return s;
}
void do_optimize(void)
{
	struct ins *start;
	int x,s,y;
	start=ins_head;
	fstart=0;
	while(start)
	{
		if(start->count_args)
		{
			if(!strcmp(start->args[0],"fun"))
			{
				fstart=start;
			}
			else if(!strcmp(start->args[0],"endf"))
			{
				fend=start;
				if(fstart)
				{
					x=0;
					while(x<8&&delete_useless_branch())
					{
						++x;
					}
					x=0;
					do
					{
						s=0;
						memset(exp_set,0,sizeof(exp_set));
						memset(exp_set2,0,sizeof(exp_set2));
						memset(exp_set3,0,sizeof(exp_set3));
						memset(exp_set4,0,sizeof(exp_set4));
						memset(exp_set5,0,sizeof(exp_set5));
						memset(exp_set6,0,sizeof(exp_set6));
						memset(exp_set7,0,sizeof(exp_set7));
						global_exp=0;
						exp_set_release();
						exp_set_init();
						s|=delete_common_exp4();
						++x;
					}
					while(x<16&&s);
					x=0;
					do
					{
						s=0;
						memset(exp_set,0,sizeof(exp_set));
						memset(exp_set2,0,sizeof(exp_set2));
						memset(exp_set3,0,sizeof(exp_set3));
						memset(exp_set4,0,sizeof(exp_set4));
						memset(exp_set5,0,sizeof(exp_set5));
						memset(exp_set6,0,sizeof(exp_set6));
						memset(exp_set7,0,sizeof(exp_set7));
						global_exp=0;
						exp_set_release();
						exp_set_init();
						s|=delete_common_exp3();
						++x;
					}
					while(x<8&&s);
					x=0;
					do
					{
						s=0;
						memset(exp_set,0,sizeof(exp_set));
						memset(exp_set2,0,sizeof(exp_set2));
						memset(exp_set3,0,sizeof(exp_set3));
						memset(exp_set4,0,sizeof(exp_set4));
						memset(exp_set5,0,sizeof(exp_set5));
						memset(exp_set6,0,sizeof(exp_set6));
						memset(exp_set7,0,sizeof(exp_set7));
						global_exp=0;
						exp_set_release();
						exp_set_init();
						s|=delete_common_exp(1);
						++x;
					}
					while(x<16&&s);
					x=0;
					do
					{
						s=0;
						memset(exp_set,0,sizeof(exp_set));
						memset(exp_set2,0,sizeof(exp_set2));
						memset(exp_set3,0,sizeof(exp_set3));
						memset(exp_set4,0,sizeof(exp_set4));
						memset(exp_set5,0,sizeof(exp_set5));
						memset(exp_set6,0,sizeof(exp_set6));
						memset(exp_set7,0,sizeof(exp_set7));
						global_exp=0;
						exp_set_release();
						exp_set_init();
						s|=delete_common_exp2();
						++x;
					}
					while(x<16&&s);
					x=0;
					while(x<16&&delete_useless_exp())
					{
						++x;
					}
				}
				fstart=0;
			}
		}
		start=start->next;
	}
}
