# -*- Mode: Python; tab-width:4 -*-

import sys

# prototype and test code for a Python AVL tree module.
# These trees can be used (simultaneously) as either a list
# or a dictionary data type, which causes a bit of a problem,
# syntax-wise:  Does tree[5] mean '5th element of <tree>' or
# "lookup key '5' in <tree>"?

class node:
	def __init__ (self, key):
		self.key	= key
		self.left	= None
		self.right	= None

	def __repr__ (self):
		return '%d' % self.key

# standard binary search tree.
class binary_tree:
	def __init__ (self):
		self.tree = None

	def insert (self, key):
		if self.tree:
			self._insert (self.tree, key)
		else:
			self.tree = node(key)

	def _insert (self, tree, key):
		if key < tree.key:
			if tree.left:
				self._insert (tree.left, key)
			else:
				tree.left = node(key)
		else:
			if tree.right:
				self._insert (tree.right, key)
			else:
				tree.right = node(key)

 	# tree-printing code inspired by
 	# Usenet article <461f7c$2ss@phoenix.csc.calpoly.edu>,
 	# posted by DSTUBBS@PHOENIX.CSC.CALPOLY.EDU (DANIEL STUBBS)
 	# 1995/10/17
 	# located by the all-powerful http://www.dejanews.com/

	def print_tree (self):
		l = link_node (None, 0, 0)
		self._print (self.tree, l)

	def _print (self, node, link):
		node_repr = repr(node)
		if node.right:
			l = link_node (link, 1, len(node_repr) + 5)
			self._print (node.right, l)
		link.print_connectors()
		sys.stdout.write ('+-[%s]' % (node_repr))
		if (node.left or node.right):
			sys.stdout.write ('-|\n')
		else:
			sys.stdout.write ('\n')
		if node.left:
			l = link_node (link, -1, len(node_repr) + 5)
			self._print (node.left, l)

def tree_from_list (list):
	bt = binary_tree()
	bt.tree = _tree_from_list (list, 0, len(list))
	return bt

def _tree_from_list (list, low, high):
	midway = ((high - low) / 2) + low
	if (midway == high == low):
		return None
	else:
		top = node(0)
		top.left = _tree_from_list (list, low, midway)
		top.key = list[midway]
		top.right = _tree_from_list (list, midway+1, high)
		return top

# used by the printing code to keep track of the node widths
# and branch directions.

class link_node:
	def __init__ (self, parent, direction, width):
		self.parent = parent
		self.direction = direction
		self.width = width

	def print_connectors (self):
		self._print_helper (self)

	def _print_helper (self, link):
		if link.parent:
			self._print_helper (link.parent)
		if link.parent and (link.parent.direction != link.direction) and (link.parent.parent):
			sys.stdout.write ('|'+ (' '*(link.width - 1)))
		else:
			sys.stdout.write (' '*link.width)

class avl_node (node):
	def __init__ (self, key, parent=None):
		self.parent = parent
		self.balance = 0
		self.rank = 1
		node.__init__ (self, key)

	def __repr__ (self):
		return '%c %s %d' % (['\\','-','/'][self.balance+1],
							 repr(self.key),
							 self.rank)

# 	import os
# 	# Colors are nice.  If these escape sequences mess up
# 	# your display, just use the __repr__  below instead.
# 	# [the new print code broke this because the escape
# 	#  codes contribute to the width, but shouldnt']
# 	if os.name == 'posix':
# 		def __repr__ (self):
# 			blue_on	= '\033[34;48m'
# 			red_on	= '\033[31;48m'
# 			reset	= '\033[0m'
# 			return (red_on +
# 					[' \ ',' - ', ' / '][self.balance+1] +
# 					# '%03d'% (self.rank) +
# 					' '+
# 					blue_on + '%03d'%self.key +
# 					reset)
# 	else:
# 		def __repr__ (self):
# 			return [' \  ',' -  ', ' /  '][self.balance+1] + '%03d'%self.key


# based directly on Knuth, Sorting and Searching, Section 6.2.3

# I prefer beautiful recursive algorithms, but in
# the interest of speed & efficiency I'll take advantage
# of code designed to run on a circa 1968 machine.
#
# The base algorithm is modified to include the use
# of the RANK field as described by Knuth, in order to
# use the tree as an ordered list data structure, where
# elements can be referenced by index.
#
# The key comparisons were also changed to permit duplicate
# keys. duplicates are inserted and deleted at the lowest
# possible position in the tree
#
# Note: I think that the <RANK> field could be used to simplify
# the rebalancing code

# Other variations:
# o use red/black trees - I've heard lots about them on Usenet, but
#   can't get my hands on any of the quoted references.
# o use threads.  threads use the wasted None pointers in all the
#   leaves to assist traversal. look at libg++'s
#   gen/AVLMap.{ccP,ccH} for an example.
#   It looks pretty hairy, but then so does this code, now.  
# o write preorder and postorder
# o make a getslice that returns a new tree rather than a list
# o make 'iterator' versions of the traversal primitives (rather than
#   building a list or tree, apply a function to each node)

Unbalanced = 'Unbalanced AVL Tree Error'
Invalid_Balance = 'Invalid Balance attribute Error'
Invalid_Rank = 'Invalid RANK attribute Error'
Invalid_Parent = 'Invalid PARENT attribute Error'

class avl_tree (binary_tree):
	def __init__ (self):

		# following Knuth, there's a special 'fake'
		# node at the top.  the real tree's root is
		# in self.tree.right.

		self.tree = avl_node (0)
		self.height = 0
		self.length = 0
		self.ops = []
		
	def insert (self, key, index=None):
		if index == None:
			self._insert (key)
		else:
			self._insert_by_index (key, index)

	def _insert (self, key):
		if not self.tree.right:
			self.tree.right = avl_node (key, self.tree)
			self.length=1
		else:
			t = self.tree
			s = p = t.right
			while 1:
				if key <= p.key:
					# move left
					p.rank = p.rank + 1
					q = p.left
					if not q:
						# insert
						q = avl_node (key, p)
						p.left = q
						break
					elif q.balance:
						t = p
						s = q
					p = q
				else:	# (key > p.key)
					# move right
					q = p.right
					if not q:
						# insert
						q = avl_node (key, p)
						p.right = q
						break
					elif q.balance:
						t = p
						s = q
					p = q
			self.length = self.length + 1
			# adjust balance factors
			if key <= s.key:
				r = p = s.left
			else:
				r = p = s.right
			while p != q:
				if key <= p.key:
					p.balance = -1
					p = p.left
				else: #  key >= p.key:
					p.balance = 1
					p = p.right
			# balancing act
			if key <= s.key:
				a = -1
			else:
				a = +1
			if not s.balance:
				s.balance = a
				self.height = self.height + 1
				return
			elif s.balance == -a:
				s.balance = 0
				return
			elif s.balance == a:
				if r.balance == a:
					# single rotation
					# -1 == left
					# +1 == right
					p = r
					if a == -1:
						s.left = r.right
						if r.right:
							r.right.parent = s
						r.right = s
						s.parent = r
						s.rank = s.rank - r.rank
					else:
						s.right = r.left
						if r.left:
							r.left.parent = s
						r.left = s
						s.parent = r
						r.rank = r.rank + s.rank
					s.balance = 0
					r.balance = 0
				elif r.balance == -a:
					# double rotation
					# -1 == left
					# +1 == right
					if a == -1:
						p = r.right
						r.right = p.left
						if p.left:
							p.left.parent = r
						p.left = r
						r.parent = p
						s.left = p.right
						if p.right:
							p.right.parent = s
						p.right = s
						s.parent = p
						p.rank = p.rank + r.rank
						s.rank = s.rank - p.rank
					else:
						p = r.left
						r.left = p.right
						if p.right:
							p.right.parent = r
						p.right = r
						r.parent = p
						s.right = p.left
						if p.left:
							p.left.parent = s
						p.left = s
						s.parent = p
						r.rank = r.rank - p.rank
						p.rank = p.rank + s.rank
					if p.balance == a:
						s.balance, r.balance = (-a, 0)
					elif p.balance == -a:
						s.balance, r.balance = (0, a)
					else:
						s.balance, r.balance = (0, 0)
					p.balance = 0
				# finishing touch
				if s == t.right:
					t.right = p
				else:
					t.left = p
				p.parent = t

	# this isn't really useful yet, because we're not
	# not using the tree to implement arbitrarily ordered
	# lists [the main reason I can think of wanting such a thing
	# is to avoid sequential allocation of lists]

	def _insert_by_index (self, key, index):
		self.length = self.length + 1
		if not self.tree:
			self.tree = avl_node (0)
			self.tree.right = avl_node (key, self.tree)
		else:
			t = self.tree
			s = p = t.right
			u = m = index
			while 1:
				if m <= p.rank:
					# move left
					p.rank = p.rank + 1
					r = p.left
					if not r:
						# insert
						q = avl_node (key, p)
						p.left = q
						break
					elif r.balance:
						t = p
						s = r
						u = m
					p = r
				else:	# (key > p.key)
					# move right
					m = m - p.rank
					r = p.right
					if not q:
						# insert
						q = avl_node (key, p)
						p.right = q
						break
					elif q.balance:
						t = p
						s = r
						u = m
					p = r
			# adjust balance factors
			m = u
			if m < s.rank:
				r = p = s.left
			else:
				r = p = s.right
			while p != q:
				if m < p.rank:
					p.balance = -1
					p = p.left
				else:
					p.balance = +1
					m = m - p.rank
					p = p.right
			# balancing act
			if u < s.rank:
				a = -1
			else:
				a = 1
			if not s.balance:
				s.balance = a
				self.height = self.height + 1
				return
			elif s.balance == -a:
				s.balance = 0
				return
			elif s.balance == a:
				if r.balance == a:
					# single rotation
					# -1 == left
					# +1 == right
					p = r
					if a == -1:
						s.left = r.right
						if r.right:
							r.right.parent = s
						r.right = s
						s.parent = r
						s.rank = s.rank - r.rank
					else:
						s.right = r.left
						if r.left:
							r.left.parent = s
						r.left = s
						s.parent = r
						r.rank = r.rank + s.rank
					s.balance = 0
					r.balance = 0
				elif r.balance == -a:
					# double rotation
					# -1 == left
					# +1 == right
					if a == -1:
						p = r.right
						r.right = p.left
						if p.left:
							p.left.parent = r
						p.left = r
						r.parent = p
						s.left = p.right
						if p.right:
							p.right.parent = s
						p.right = s
						s.parent = p
						p.rank = p.rank + r.rank
						s.rank = s.rank - p.rank
					else:
						p = r.left
						r.left = p.right
						if p.right:
							p.right.parent = r
						p.right = r
						r.parent = p
						s.right = p.left
						if p.left:
							p.left.parent = s
						p.left = s
						s.parent = p
						r.rank = r.rank - p.rank
						p.rank = p.rank + s.rank
					if p.balance == a:
						s.balance, r.balance = (-a, 0)
					elif p.balance == -a:
						s.balance, r.balance = (0, a)
					else:
						s.balance, r.balance = (0, 0)
					p.balance = 0
				# finishing touch
				if s == t.right:
					t.right = p
				else:
					t.left = p
				p.parent = t

	# I want to use these, but currently don't
	def single_rotate_right	(self, s, r):
		s.right = r.left
		if r.left:
			r.left.parent = s
		r.left = s
		s.parent = r
		r.rank = r.rank + s.rank

	def single_rotate_left	(self, s, r):
		s.left = r.right
		if r.right:
			r.right.parent = s
		r.right = s
		s.parent = r
		s.rank = s.rank - r.rank

	# this is very much like the code for insert, 'cept I wrote it
	# myself.

	def remove (self, key):
		x = self.tree.right
		while 1:
			if key < x.key:
				# move left
				# we will be deleting from the left, adjust
				# this node's rank accordingly.
				x.rank = x.rank - 1
				if x.left:
					x = x.left
				else:
					# Oops! now we have to undo the rank changes
					# all the way up the tree
					x.rank = x.rank + 1
					while x != self.tree.right:
						if x.parent.left == x:
							x.parent.rank = x.parent.rank + 1
						x = x.parent
					raise KeyError, 'key not in tree'
			elif key > x.key:
				# move right
				if x.right:
					x = x.right
				else:
					x.rank = x.rank + 1
					while x != self.tree.right:
						if x.parent.left == x:
							x.parent.rank = x.parent.rank + 1
						x = x.parent
					raise KeyError, 'key not in tree'
			else:
				break

		shift = 0
		# <x> is the node we wish to delete
		if (x.left and x.right):
			
			# debug only
			#print 'shifting...'
			shift = 1

			# the complicated case.  reduce this
			# to the simple case where we are deleting
			# a node with only a single child.
			# find the immediate predecessor <y>
			y = x.left
			while y.right:
				y = y.right
			# replace <x> with <y>
			x.key = y.key
			# we know <x>'s left subtree lost a node because
			# that's where we took it from.
			x.rank = x.rank - 1
			x = y
		# now <x> has at most one child
		# now we scoot this child into the place of <x>
		# is <x> a left (xp=1) or right (xp=0) child of its parent?
		xp = (x == x.parent.left)
		if x.left:
			x_child = x.left
			x_child.parent = x.parent
		elif x.right:
			x_child = x.right
			x_child.parent = x.parent
		else:
			x_child = None
		if xp:
			x.parent.left = x_child
			shortened_side = -1
		else:
			x.parent.right = x_child
			shortened_side = +1
		# debug only
		#if shift:
		#	self.print_tree()
		#	print '-'*50

		# the height of the subtree <x>
		# has now been shortened.  climb back up
		# the tree, rotating to adjust for the change
		shorter = 1
		p = x.parent
		# p = x

		while shorter and p.parent:

			#print p
			#raw_input()

			# case 1: height unchanged
			if p.balance == 0:
				#print 'case 1'
				if shortened_side == -1:
					# we removed a left child, the tree is
					# now heavier on the right
					p.balance = +1
				else:
					# we removed a right child, the tree is
					# now heavier on the left.
					p.balance = -1
				shorter = 0
			# case 2: taller subtree shortened, height reduced
			elif p.balance == shortened_side:
				#print 'case 2'
				p.balance = 0
			# case 3: shorter subtree shortened
			else:
				top = p.parent
				# set <q> to the taller of the two subtrees of <p>
				if shortened_side == 1:
					q = p.left
				else:
					q = p.right
				if q.balance == 0:
					#print 'case 3a'
					# case 3a: height unchanged
					if shortened_side == -1:
						# single rotate left
						q.parent = p.parent
						p.right = q.left
						if q.left:
							q.left.parent = p
						q.left = p
						p.parent = q
						q.rank = q.rank + p.rank
					else:
						# single rotate right
						q.parent = p.parent
						p.left = q.right
						if q.right:
							q.right.parent = p
						q.right = p
						p.parent = q
						p.rank = p.rank - q.rank
					shorter = 0
					q.balance = shortened_side
					p.balance = (- shortened_side)
				elif q.balance == p.balance:
					#print 'case 3b'
					# case 3b: height reduced
					if shortened_side == -1:
						# single rotate left
						q.parent = p.parent
						p.right = q.left
						if q.left:
							q.left.parent = p
						q.left = p
						p.parent = q
						q.rank = q.rank + p.rank
					else:
						# single rotate right
						q.parent = p.parent
						p.left = q.right
						if q.right:
							q.right.parent = p
						q.right = p
						p.parent = q
						p.rank = p.rank - q.rank
					shorter = 1
					q.balance = 0
					p.balance = 0
				else:
					#print 'case 3c'
					# case 3c: height reduced, balance factors
					# opposite.
					if shortened_side == 1:
						# double rotate right
						# first, a left rotation around q
						r = q.right
						r.parent = p.parent
						q.right = r.left
						if r.left:
							r.left.parent = q
						r.left = q
						q.parent = r
						# now, a right rotation around p
						p.left = r.right
						if r.right:
							r.right.parent = p
						r.right = p
						p.parent = r
						r.rank = r.rank + q.rank
						p.rank = p.rank - r.rank
					else:
						# double rotate left
						# first, a right rotation around q
						r = q.left
						r.parent = p.parent
						q.left = r.right
						if r.right:
							r.right.parent = q
						r.right = q
						q.parent = r
						# now, a left rotation around p
						p.right = r.left
						if r.left:
							r.left.parent = p
						r.left = p
						p.parent = r
						q.rank = q.rank - r.rank
						r.rank = r.rank + p.rank
					# (shortened_side == 1) <=> -a
					# this is magic
					if r.balance == shortened_side:
						q.balance, p.balance = (-shortened_side, 0)
					elif r.balance == (- shortened_side):
						q.balance, p.balance = (0, shortened_side)
					else:
						q.balance, p.balance = (0, 0)
					r.balance = 0
					q = r
				# a rotation has caused <q> (or <r> in case 3c) to become
				# the root.  let <p>'s former parent know this.
				if top.left == p:
					top.left = q
				else:
					top.right = q
				# end case 3
				p = q
			#self.print_tree()
			x = p
			p = x.parent
			# shortened_side tells us which side we
			# came up from
			if x == p.left:
				shortened_side = -1
			else:
				shortened_side = +1
			# end while (shorter)
		# when we're all done, we're one shorter.
		self.length = self.length - 1

	def __len__ (self):
		return self.length

	def __getitem__ (self, index):
		m = index + 1
		p = self.tree.right
		while 1:
			if not p:
				raise IndexError, 'index out of range'
			if m < p.rank:
				p = p.left
			elif m > p.rank:
				m = m - p.rank
				p = p.right
			else:
				return p.key

	def count_prev_ops (self):
		# find the rightmost (last) node
		node = self.tree.right
		while node.right:
			node = node.right
		for i in xrange(self.length):
			node = self._get_prev (node)
		return self.ops

	# analysis:
	# I built random trees of 1000, 10000, and 100000
	# nodes.  I then iterated get_prev() over the whole
	# tree, giving me a list of the number of ops for
	# each call to get_prev().  In every case
	# after sorting I get a sequence where 1/2 of the calls
	# required only 1 operation, 1/4 required 2 ops, 1/8
	# required 3, etc..
	# assuming that this is an accurate characterization,
	# the total number of operations to scan the
	# whole tree is
	# (1 * (1/2 * n) +
	# (2 * (1/4 * n)) +
	# (3 * (1/8 * n)) +
	# (4 * (1/16 * n)) +
	# ...
	# k n / (2^k)
	# 
	# this sum converges on 2 as n->infinity, but I don't have
	# any symbolic math programs to verify it (and it's been
	# 8 years since I did this type of calculus 8^)
	# 
	# statistically, the sum seems to hover near 2*n,
	# which makes the average number of operations to find
	# the predecessor/successor of a node ~2.  I'd love to
	# prove this.

	# used by getslice only, that's why there's no fall-off check
	def _get_prev (self, node):
		# immediate predecessor is either
		# a) one left, down until no right child
		# b) up until previous node was a right child.
		ops = 0
		if node.left:
			node = node.left
			ops = ops + 1
			while node.right:
				node = node.right
				ops = ops + 1
			self.ops.append (ops)
			return node
		else:
			child = node
			while node.parent:
				node = node.parent
				ops = ops + 1
				if child == node.right:
					self.ops.append (ops)
					return node
				child = node
			self.ops.append (ops)
			return node

	def _get_succ (self, node):
		# immediate predecessor is either
		# a) one left, down until no right child
		# b) up until previous node was a right child.
		ops = 0
		if node.right:
			node = node.right
			ops = ops + 1
			while node.left:
				node = node.left
				ops = ops + 1
			self.ops.append (ops)
			return node
		else:
			child = node
			while node.parent:
				node = node.parent
				ops = ops + 1
				if child == node.left:
					self.ops.append (ops)
					return node
				child = node
			self.ops.append (ops)
			return node

	# Return a slice of the tree as a list [rather than as a new tree]

	def __getslice__ (self, i=None, j=None):
		print "?",i,j,self.length
		if i<0: i=0
		if j>self.length: j=self.length
		print i,j,self.length
			
		# result list template
		num_left = (j - i)
		result = range (num_left)

		# find the <j>th node.
		m = j + 1
		node = self.tree.right
		while 1:
			if not node:
				raise IndexError, 'index out of range'
			if m < node.rank:
				node = node.left
			elif m > node.rank:
				m = m - node.rank
				node = node.right
			else:
				break

		# fill in the result list by repeatedly
		# calling self._get_prev()
		while num_left:
			#print node
			num_left = num_left - 1
			node = self._get_prev(node)
			result[num_left] = node.key
		return result

	def __repr__ (self):
		return repr(self.inorder())

	def inorder (self):
		if not self.tree.right:
			return []
		return self._inorder (self.tree.right)

	def _inorder (self, node):
		result = []
		if node.left:
			result = result + self._inorder (node.left)
		result.append (node.key)
		if node.right:
			result = result + self._inorder (node.right)
		return result

	def verify (self):
		self._verify_balance	(self.tree.right)
		self._verify_rank		(self.tree.right)
		self._verify_parent		(self.tree.right, self.tree)

	# this verifies balance 'manually', and also
	# double-checks each node's <balance> member.
	def _verify_balance (self, node):
		if node == None:
			return 0
		lh = self._verify_balance (node.left)
		rh = self._verify_balance (node.right)
		if (rh - lh) != node.balance:
			raise Invalid_Balance, 'at node <%s>' % (repr(node))
		if abs (lh - rh) > 1:
			raise Unbalanced, 'at node <%s>' % (repr(node))
		else:
			return (1 + max (lh, rh))

	def _verify_rank (self, node):
		if not node:
			return 0
		num_left = num_right = 0
		if node.left:
			num_left = self._verify_rank (node.left)
		if node.right:
			num_right = self._verify_rank (node.right)
		if node.rank != num_left + 1:
			raise Invalid_Rank, 'at node <%s>' % (repr(node))
		return num_left + num_right + 1

	def _verify_parent (self, node, parent):
		if node.parent != parent:
			raise Invalid_Parent, 'at node <%s>' % (repr(node))
		if node.left:
			self._verify_parent (node.left, node)
		if node.right:
			self._verify_parent (node.right, node)

demo_nums = [50, 45, 15, 10, 75, 55, 70, 80, 60, 32, 20, 40, 25, 22, 31, 30]

def avl_demo():
	tree = avl_tree()
	for num in demo_nums:
		tree.insert (num)
		print '-'*50
		tree.print_tree()
		print '<cr>',
		raw_input()
	return tree


def btest(n=50):
	print 'generating random numbers...'
	nums = map (lambda x,n=n: int(random.uniform (0, 999)), range(n))
	print nums
	t = binary_tree()
	print 'inserting them into the tree...'
	for i in range(len(nums)):
		num = nums[i]
		t.insert (num)
	t.print_tree()

def del_test (iter=100, l=20):
	for i in range (iter):
		at, nums = test (l)
		dnums = []
		while at:
			n = random.choice (at)
			print n
			dnums.append (n)
			at.remove(n)
			at.verify()
			print '-' * 50
			at.print_tree()
		print '*'*50
		print '*'*50

def slice_test (tree_size=100, times=100):
	at = test(tree_size)
	for x in range(times):
		print x
		i = int (random.uniform (0, tree_size-1))
		j = int (random.uniform (0, tree_size-1))
		if j < i:
			i, j = j, i
		sl = at[i:j]
	return at.ops

# The rest of the file was used while working on the
# tree_from_tree slice algorithm

class tree_iterator:
	def __init__ (self, tree, node=None):
		self.tree = tree
		# default to the leftmost node
		if node == None:
			node = tree.tree
			while node.left:
				node = node.left
		self.node = node

	def next (self):
		result = self.node
		self.node = self.tree._get_succ (self.node)
		return result


# strong typing is for weak minds

def get_slice_tree (tree, i, j):
	# find the <i>th node.
	m = i + 1
	node = tree.tree.right
	while 1:
		if not node:
			raise IndexError, 'index out of range'
		if m < node.rank:
			node = node.left
		elif m > node.rank:
			m = m - node.rank
			node = node.right
		else:
			break
	distance = j - i
	iter = tree_iterator (tree, node)
	result = avl_tree()
	result.insert(0)
	result.tree.right = build_tree (iter, 0, distance)
	return result


def build_tree (iter, low, high):
	midway = ((high - low) / 2) + low
	if (midway == high == low):
		return None
	else:
		top = avl_node(0)
		top.left = build_tree (iter, low, midway)
		if top.left:
			top.left.parent = top
		top.key = iter.next().key
		top.right = build_tree (iter, midway+1, high)
		if top.right:
			top.right.parent = top
		return top
