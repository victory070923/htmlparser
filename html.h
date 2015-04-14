#ifndef HTML_H
#define HTML_H

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "node.h"
using namespace std;
typedef const char* Iter;
class Html
{
	public:
		Html(){
			mCurrentState = mHtmlTree.root;
		}
		~Html(){
			mHtmlTree.destory();
		}
		void parseHtml(Iter str,Iter end);
		
		void parseHtmlTag(Iter str,Iter end);
		void parseContent(Iter str,Iter end);
		void parseComment(Iter str, Iter end);
		
		Iter skipHtmlTag(Iter str, Iter end);
		Iter skipHtmlComment(Iter str, Iter end);
		
		TNode_Iter appendTag(Node_Iter node, bool isEnd);
		TNode_Iter appendNode(Node_Iter iter,Node_Iter node);	
			
		friend ostream &operator<<(ostream &stream, Html &tr);
	private:
		//list<Node*> nodelist;
		Tree mHtmlTree;
		TNode_Iter  mCurrentState;
		unsigned long mCurrentOffset;
};

ostream &operator<<(ostream &stream, Html &tr)
{
	tr.mHtmlTree.level_order_vist(stream);
	return stream;
}

void Html::parseHtml(Iter begin,Iter end)
{
	mCurrentOffset = 0;
	while (begin != end)
	{
		Iter c(begin);
		while (c != end)
		{
			if (*c == '<')
			{
				Iter d(c);
				++d;
				if (d != end)
				{
					if (isalpha(*d))
					{
						// begin tag
						if (begin != c)
							parseContent(begin, c);
						d = skipHtmlTag(d, end);
						parseHtmlTag(c, d);					
						c = d;
						begin = c;
						break;
					}
					if (*d == '/')
					{
						if (begin != c)
							parseContent(begin, c);

						Iter e(d);
						++e;
						if (e != end && isalpha(*e))
						{
							// end  tag
							d = skipHtmlTag(d, end);
							parseHtmlTag(c, d);
						}
						else
						{
							// not a matched end tag, treat as comment
							d = skipHtmlTag(d, end);
							parseComment(c, d);
						}

						// continue
						c = d;
						begin = c;
						break;
					}

					if (*d == '!')
					{
						// comment
						if (begin != c)
							parseContent(begin, c);

						Iter e(d);
						++e;

						if (e != end && *e == '-' && ++e != end && *e == '-')
						{
							++e;
							d = skipHtmlComment(e, end);
						}
						else
						{
							d = skipHtmlTag(d, end);
						}

						parseComment(c, d);

						// continue
						c = d;
						begin = c;
						break;
					}					
				}
			}
			c++;
		}

		//  text content at end
		if (begin != c)
		{
			parseContent(begin, c);
			begin = c;
		}
	}
	return;
}
void Html::parseContent(Iter b, Iter c)
{
	Node_Iter pnode = new Node;
	string txt(b, c);
	pnode->tagName(txt);
	pnode->text(txt);
	pnode->offset(mCurrentOffset);
	pnode->length((unsigned int)txt.length());
	pnode->isTag(false);
	pnode->isComment(false);

	mCurrentOffset += pnode->length();
	mHtmlTree.appendNode(mCurrentState,pnode);
}

void Html::parseComment(Iter b, Iter c)
{
	
	Node_Iter pnode = new Node;
	string comment(b, c);
	pnode->tagName(comment);
	pnode->text(comment);
	pnode->offset(mCurrentOffset);
	pnode->length((unsigned int)comment.length());
	pnode->isTag(false);
	pnode->isComment(true);

	mCurrentOffset += pnode->length();
	mHtmlTree.appendNode(mCurrentState,pnode);
}

void Html::parseHtmlTag(Iter b, Iter c)
{
	Iter name_begin(b);
	++name_begin;
	bool is_end_tag = (*name_begin == '/');
	if (is_end_tag) ++name_begin;

	Iter name_end(name_begin);
	while (name_end != c && isalnum(*name_end)) 
	{
		++name_end;
	}

	string name(name_begin, name_end);
	Node_Iter tag_node = new Node;

	string text(b, c);
	tag_node->length(static_cast<unsigned int>(text.length()));
	tag_node->tagName(name);
	tag_node->text(text);
	tag_node->offset(mCurrentOffset);
	tag_node->isTag(true);
	tag_node->isComment(false);
	mCurrentOffset += tag_node->length();
	appendTag(tag_node, is_end_tag);
}


TNode_Iter Html::appendTag(Node_Iter node, bool isEnd)
{
	if (!isEnd) 
	{
		//append to current tree node
		TNode_Iter next_state;
		next_state = mHtmlTree.appendNode(mCurrentState, node);
		mCurrentState = next_state;
	} 
	else 
	{
		vector< TNode_Iter> path;
		TNode_Iter i = mCurrentState;
		bool found_open = false;
		while (i && i != mHtmlTree.begin())
		{
			bool equal;
			const char *open = i->pdata->tagName().c_str();
			const char *close = node->tagName().c_str();
			equal = !(stricmp(open,close));
			if (equal) 
			{
				i->pdata->length(node->offset() + node->length() - i->pdata->offset());
				i->pdata->closingText(node->text());

				mCurrentState = mHtmlTree.parent(i);
				found_open = true;
				break;
			} 
			else 
			{
				path.push_back(i);
			}

			i = mHtmlTree.parent(i);
		}

		if (found_open)
		{
			//if exist no match child, move up
			for (unsigned int j = 0; j < path.size(); ++j)
			{
				mHtmlTree.moveChildToUplevel(path[j]);
			}
		} 
		else 
		{
			// no matched open tag,treat as comment
			node->isTag(false);
			node->isComment(true);
			mHtmlTree.appendNode(mCurrentState, node);
		}
	}
}

Iter Html::skipHtmlComment(Iter c, Iter end)
{
	while ( c != end ) {
		if (*c++ == '-' && c != end && *c == '-')
		{
			Iter d(c);
			while (++c != end && isspace(*c));
			if (c == end || *c++ == '>') break;
			c = d;
		}
	}

	return c;
}

Iter Html::skipHtmlTag(Iter c, Iter end)
{
	while (c != end && *c != '>')
	{
		if (*c != '=') 
		{
			++c;
		}
		else
		{ // found an attribute,in case attr value contain '>'
			++c;
			while (c != end && isspace(*c)) ++c;

			if (c == end) break;

			if (*c == '\"' || *c == '\'') 
			{
				Iter save(c);
				char quote = *c++;
				c = Utils::find_next_char(c, end, quote);
				if (c != end) 
				{
					++c;
				} 
				else 
				{//no matched quote,treat as noraml, continue at next save point.
					c = save;
					++c;
				}
			}
		}
	}

	if (c != end) ++c;
	
	return c;
}

#endif
