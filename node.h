#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <string>
#include <deque>
#include <map>

#include "utils.h"
using namespace std;

class Node;
class TreeNode;

typedef Node* Node_Iter;
typedef TreeNode* TNode_Iter;
typedef map<string, string>::iterator Att_Iter;

class Node
{
	public:
		Node(string txt):mText(txt){	}
		Node(){}
		operator string() const;
		ostream &operator<<(ostream &stream) const;
		map<string,string>& attributes()  { return mAttributes; }
		void parseAttributes();
		bool operator==(const Node &rhs) const;
		inline void text(const string& text) { mText = text; }
		inline const string& text() const { return mText; }

		inline void closingText(const string &text) { mClosingText = text; }
		inline const string& closingText() const { return mClosingText; }

		inline void offset(unsigned int offset) { mOffset = offset; }
		inline unsigned int offset() const { return mOffset; }

		inline void length(unsigned int length) { mLength = length; }
		inline unsigned int length() const { return mLength; }

		inline void tagName(const string& tagname) { mTagName = tagname; }
		inline const string& tagName() const { return mTagName; }

		bool isTag() const { return mIsHtmlTag; }
		void isTag(bool is_html_tag){ mIsHtmlTag = is_html_tag; }

		bool isComment() const { return mComment; }
		bool isContent() const { return !mComment && !mIsHtmlTag; }
		void isComment(bool comment){ mComment = comment; }
		


	protected:
		//list<Node*> nodelist;		
		string mText;
		string mClosingText;
		unsigned int mOffset;
		unsigned int mLength;
		string mTagName;
		map<string, string> mAttributes;
		bool mIsHtmlTag;
		bool mComment;	
};

class TreeNode
{
	public:
		TreeNode(){
		parent = first_child =last_child = prev_sibling =next_sibling =NULL;
		pdata = NULL;
		mVisit = 0;
		}
		
		TreeNode*parent,*first_child, *last_child,*prev_sibling, *next_sibling;

		bool hasVist() const { return mVisit; }
		void hasVist(bool bvisit){ mVisit = bvisit; }
		bool mVisit;
		Node* pdata;		
};

class Tree
{
	public:
		Tree():root(NULL){root = new TreeNode;}
		TNode_Iter begin(){	return root;}
		TNode_Iter parent(TNode_Iter i){if(i)return i->parent;	return NULL;}
		TNode_Iter appendNode(TNode_Iter pos,Node_Iter pnode);
		TNode_Iter moveChildToUplevel(TNode_Iter pos);
		TNode_Iter root;
		void level_order_vist(ostream &stream);
		void visit(ostream &stream,TNode_Iter p);
		void destory();
};

TNode_Iter Tree::appendNode(TNode_Iter treepos,Node_Iter pnode)
{
   TNode_Iter tmp = new TreeNode;
   tmp->first_child=0;
   tmp->last_child=0;
   tmp->pdata = pnode;

   tmp->parent=treepos;
   if(treepos->last_child!=0) {
      treepos->last_child->next_sibling=tmp;
      }
   else {
      treepos->first_child=tmp;
      }
   tmp->prev_sibling=treepos->last_child;
   treepos->last_child=tmp;
   tmp->next_sibling=0;
   return tmp;
}

TNode_Iter Tree::moveChildToUplevel(TNode_Iter pos)
{
   if(pos->first_child==0)
      return pos;

   TNode_Iter tmp=pos->first_child;
   while(tmp) {
      tmp->parent=pos->parent;
      tmp=tmp->next_sibling;
      } 
   if(pos->next_sibling) {
      pos->last_child->next_sibling=pos->next_sibling;
      pos->next_sibling->prev_sibling=pos->last_child;
      }
   else {
      pos->parent->last_child=pos->last_child;
      }
   pos->next_sibling=pos->first_child;
   pos->next_sibling->prev_sibling=pos;
   pos->first_child=0;
   pos->last_child=0;

   return pos;
}
void Tree::destory()
{
	TNode_Iter tmp = root;
	deque<TNode_Iter> que1;
	que1.push_back(root);
	
	while(!que1.empty())
	{	
		tmp =que1.front();		
		que1.pop_front();							
		for(TNode_Iter i=tmp->first_child;i!=NULL;i = i->next_sibling)
		{					
			que1.push_back(i);
		}
		delete tmp->pdata;	
		delete tmp;		
	}	
}
void Tree::level_order_vist(ostream &stream)
{
	TNode_Iter tmp = root;
	deque<TNode_Iter> que1,que2;
	que1.push_back(root);
	int level = 1;
	while(!que1.empty())
	{
		while(!que1.empty())
		{
			tmp =que1.front();
			visit(stream,tmp);
			que1.pop_front();
								
			for(TNode_Iter i=tmp->first_child;i!=NULL;i = i->next_sibling)
			{					
				que2.push_back(i);
			}
			
		}		
		swap(que1,que2);	
		if(!que1.empty())	
			stream<<endl<<"++++++++++++++++++  level "<< level++ <<"   ++++++++++++++++++ "<<endl;
	}	
}
void Tree::visit(ostream &stream,TNode_Iter p)
{
	if(!p ||!p->pdata||p==root)return;
	stream<<" --------------------- "<<endl;
	stream<<"isTag: "<<  p->pdata->isTag()<<"  isTxt: "<<  p->pdata->isContent()<<"  isComment: "<<  p->pdata->isComment()<<endl;
	string ptag ="ROOT";
	if(p->parent&&p->parent->pdata) ptag = p->parent->pdata->tagName();
	if( p->pdata->isTag())
	{		
		stream<<"  TagName: "<<p->pdata->tagName()<<"  ParentTagName: "<<ptag<<endl;
		p->pdata->parseAttributes();
		stream<<"  Attributes:"<<endl;
		for(Att_Iter i = p->pdata->attributes().begin();i!=p->pdata->attributes().end();++i)
			stream<<"     "<<i->first<<"   =   "<<i->second<<endl;		
	}
	string ptxt = p->pdata->text();
	if(Utils::IsGraph(ptxt.c_str()))	ptxt = "Blank Txt";
	if( p->pdata->isContent())
		stream<<"  Text: "<<ptxt<<"  ParentTagName: "<<ptag<<endl;
	if( p->pdata->isComment())
		stream<<"  Text: "<<ptxt<<endl;
	
}

void Node::parseAttributes()
{
	if (!(isTag())) return;
	
	const char *end;	string tmp;
	const char *ptr = mText.c_str();
	const char *ptrtmp;
	if ((ptr = strchr(ptr, '<')) == 0) return;
	++ptr;
	ptr = Utils::SkipBlank(ptr);
	if (!isalpha(*ptr)) return;
	ptr = Utils::SkipValidNames(ptr);//tag name
	ptr = Utils::SkipBlank(ptr);
	
	while (*ptr && *ptr != '>') 
	{
		string key, val;
		ptr = Utils::SkipNotALpnum(ptr); // garbage 

		end = ptr;	
		end = Utils::SkipValidNames(ptr); 
		if(end>ptr)
			key.assign(ptr, end);		
		ptr = end;
		ptr = Utils::SkipBlank(ptr);
		if (*ptr == '=') 
		{
			++ptr;
			ptr = Utils::SkipBlank(ptr);
			if (*ptr == '"' || *ptr == '\'') 
			{
				char quote = *ptr;
				const char *end = strchr(ptr + 1, quote);
				if (end == 0)
				{					
					const char *end1, *end2;
					end1 = strchr(ptr + 1, ' ');
					end2 = strchr(ptr + 1, '>');
					if (end1 && end1 < end2) end = end1;
					else end = end2;
					if (end == 0) return;
				}
				const char *begin = ptr + 1;
				begin = Utils::SkipBlank(begin);
				
				const char *endreal =end -1;//from back to rm blank
				while (isspace(*endreal) && endreal >= begin) --endreal;
				if(begin<endreal)
					val.assign(begin, endreal+1);
				ptr = end + 1;
			}
			else 
			{
				end = Utils::SkipNotBlank(ptr,strchr(ptr, '>'));				
				val.assign(ptr, end);
				ptr = end;
			}
			mAttributes.insert(make_pair(key, val));
		}
		else
		{
			mAttributes.insert(make_pair(key, string()));
		}
	}
	
}
#endif
