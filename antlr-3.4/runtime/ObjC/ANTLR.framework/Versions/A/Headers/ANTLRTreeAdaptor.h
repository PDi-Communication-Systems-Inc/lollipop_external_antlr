// [The "BSD licence"]
// Copyright (c) 2006-2007 Kay Roepke 2010 Alan Condit
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. The name of the author may not be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#import "ANTLRToken.h"
#import "ANTLRBaseTree.h"
#import "ANTLRTokenStream.h"

#pragma warning tree/node diction is broken.

@protocol ANTLRTreeAdaptor <NSObject, NSCopying>

#pragma mark Construction

+ (id<ANTLRTree>) newEmptyTree;

- (id<ANTLRTree>) createTree:(id<ANTLRToken>)payload;

#pragma mark ANTLRTreeAdaptor implementation
- (id<ANTLRTree>)dupNode:(id<ANTLRTree>)aNode;	// copies just the node
- (id<ANTLRTree>)dupTree:(id<ANTLRTree>)aTree;	// copies the entire subtree, recursively

/** Return a nil node (an empty but non-null node) that can hold
 *  a list of element as the children.  If you want a flat tree (a list)
 *  use "t=adaptor.nil(); t.addChild(x); t.addChild(y);"
 */
- (id) emptyNode;

/** Return a tree node representing an error.  This node records the
 *  tokens consumed during error recovery.  The start token indicates the
 *  input symbol at which the error was detected.  The stop token indicates
 *  the last symbol consumed during recovery.
 *
 *  You must specify the input stream so that the erroneous text can
 *  be packaged up in the error node.  The exception could be useful
 *  to some applications; default implementation stores ptr to it in
 *  the CommonErrorNode.
 *
 *  This only makes sense during token parsing, not tree parsing.
 *  Tree parsing should happen only when parsing and tree construction
 *  succeed.
 */
- (id) errorNode:(id<ANTLRTokenStream>)anInput
            From:(id<ANTLRToken>)aStartToken
              To:(id<ANTLRToken>)aStopToken
       Exception:(NSException *) e;

/** Is tree considered a nil node used to make lists of child nodes? */
- (BOOL) isNil:(id<ANTLRTree>)aTree;


- (void) addChild:(id<ANTLRTree>)child toTree:(id<ANTLRTree>)aTree;

/** If oldRoot is a nil root, just copy or move the children to newRoot.
 *  If not a nil root, make oldRoot a child of newRoot.
 *
 *    old=^(nil a b c), new=r yields ^(r a b c)
 *    old=^(a b c), new=r yields ^(r ^(a b c))
 *
 *  If newRoot is a nil-rooted single child tree, use the single
 *  child as the new root node.
 *
 *    old=^(nil a b c), new=^(nil r) yields ^(r a b c)
 *    old=^(a b c), new=^(nil r) yields ^(r ^(a b c))
 *
 *  If oldRoot was null, it's ok, just return newRoot (even if isNil).
 *
 *    old=null, new=r yields r
 *    old=null, new=^(nil r) yields ^(nil r)
 *
 *  Return newRoot.  Throw an exception if newRoot is not a
 *  simple node or nil root with a single child node--it must be a root
 *  node.  If newRoot is ^(nil x) return x as newRoot.
 *
 *  Be advised that it's ok for newRoot to point at oldRoot's
 *  children; i.e., you don't have to copy the list.  We are
 *  constructing these nodes so we should have this control for
 *  efficiency.
 */
- (id) becomeRoot:(id<ANTLRTree>)newRoot old:(id<ANTLRTree>)oldRoot;

- (id) rulePostProcessing:(id<ANTLRTree>)root;

#pragma mark Rewrite Rules
                           
- (NSUInteger) getUniqueID:(id<ANTLRTree>)aNode;

- (id<ANTLRTree>) createTree:(NSInteger)tokenType FromToken:(id<ANTLRToken>)fromToken;
- (id<ANTLRTree>) createTree:(NSInteger)tokenType FromToken:(id<ANTLRToken>)fromToken Text:(NSString *)text;
- (id<ANTLRTree>) createTree:(NSInteger)tokenType Text:(NSString *)text;

#pragma mark Content

- (id<ANTLRTree>)dupNode:(id<ANTLRTree>)aNode;
- (id<ANTLRTree>)dupTree:(id<ANTLRTree>)aTree;

- (NSInteger) getType:(id<ANTLRTree>)aNode;
- (void) setType:(id<ANTLRTree>)aNode Type:(NSInteger)tokenType;

- (NSString *) getText:(id<ANTLRTree>)aNode;
- (void) setText:(id<ANTLRTree>)aNode Text:(NSString *)tokenText;

- (id<ANTLRToken>) getToken:(id<ANTLRTree>)t;

- (void) setTokenBoundaries:(id<ANTLRTree>)aTree From:(id<ANTLRToken>)startToken To:(id<ANTLRToken>)stopToken;
- (NSInteger) getTokenStartIndex:(id<ANTLRTree>)aTree;
- (NSInteger) getTokenStopIndex:(id<ANTLRTree>)aTree;

#pragma mark Navigation / Tree Parsing

/** Get a child 0..n-1 node */
- (id<ANTLRTree>) getChild:(id<ANTLRTree>)aNode At:(NSInteger) i;
/** Set ith child (0..n-1) to t; t must be non-null and non-nil node */
- (void) setChild:(id<ANTLRTree>)aTree At:(NSInteger)index Child:(id<ANTLRTree>)child;
/** Remove ith child and shift children down from right. */
- (id<ANTLRTree>) deleteChild:(id<ANTLRTree>)t Index:(NSInteger)index;

/** How many children?  If 0, then this is a leaf node */
- (NSInteger) getChildCount:(id<ANTLRTree>) aTree;

/** Who is the parent node of this node; if null, implies node is root.
 *  If your node type doesn't handle this, it's ok but the tree rewrites
 *  in tree parsers need this functionality.
 */
- (id<ANTLRTree>)getParent:(id<ANTLRTree>)t;
- (void) setParent:(id<ANTLRTree>)t With:(id<ANTLRTree>)parent;

/** What index is this node in the child list? Range: 0..n-1
 *  If your node type doesn't handle this, it's ok but the tree rewrites
 *  in tree parsers need this functionality.
 */
- (NSInteger) getChildIndex:(id<ANTLRTree>)t;
- (void) setChildIndex:(id<ANTLRTree>)t With:(NSInteger)index;

- (void) replaceChildren:(id<ANTLRTree>)parent From:(NSInteger)startChildIndex To:(NSInteger)stopChildIndex With:(id<ANTLRTree>)t;

@end
