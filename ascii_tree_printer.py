class AsciiNode:
    def __init__(self, val):
        self.left = self.right = None
        self.edge_length = 0
        self.height = 0
        self.label = str(val)
        self.parent_dir = 0  # -1 = left, 0 = root, 1 = right

    @property
    def lablen(self):
        return len(self.label)

class AsciiTreePrinter:
    MAX_HEIGHT = 1000

    def __init__(self, gap=3):
        self.gap = gap
        self.lprofile = [float('inf')] * self.MAX_HEIGHT
        self.rprofile = [-float('inf')] * self.MAX_HEIGHT
        self.print_next = 0

    def print_tree(self, root):
        if not root:
            return

        proot = self.build_ascii_tree(root)
        self.compute_edge_lengths(proot)

        self.lprofile = [float('inf')] * self.MAX_HEIGHT
        self.compute_lprofile(proot, 0, 0)

        xmin = min(self.lprofile[:proot.height])
        for i in range(proot.height):
            self.print_next = 0
            self.print_level(proot, -xmin, i)
            print()

        if proot.height >= self.MAX_HEIGHT:
            print(f"(This tree is taller than {self.MAX_HEIGHT}, and may be drawn incorrectly.)")

    def build_ascii_tree(self, root):
        if not root:
            return None
        # Access the first 3 variables: val, left, right using dynamic attributes
        node = AsciiNode(getattr(root, 'val', None))
        node.left = self.build_ascii_tree(getattr(root, 'left', None))
        node.right = self.build_ascii_tree(getattr(root, 'right', None))
        if node.left:
            node.left.parent_dir = -1
        if node.right:
            node.right.parent_dir = 1
        return node

    def compute_lprofile(self, node, x, y):
        if not node:
            return
        self.lprofile[y] = min(self.lprofile[y], x - (node.lablen - (node.parent_dir == -1)) // 2)
        if node.left:
            for i in range(1, node.edge_length + 1):
                if y + i < self.MAX_HEIGHT:
                    self.lprofile[y + i] = min(self.lprofile[y + i], x - i)
        self.compute_lprofile(node.left, x - node.edge_length - 1, y + node.edge_length + 1)
        self.compute_lprofile(node.right, x + node.edge_length + 1, y + node.edge_length + 1)

    def compute_rprofile(self, node, x, y):
        if not node:
            return
        self.rprofile[y] = max(self.rprofile[y], x + (node.lablen - (node.parent_dir != -1)) // 2)
        if node.right:
            for i in range(1, node.edge_length + 1):
                if y + i < self.MAX_HEIGHT:
                    self.rprofile[y + i] = max(self.rprofile[y + i], x + i)
        self.compute_rprofile(node.left, x - node.edge_length - 1, y + node.edge_length + 1)
        self.compute_rprofile(node.right, x + node.edge_length + 1, y + node.edge_length + 1)

    def compute_edge_lengths(self, node):
        if not node:
            return
        self.compute_edge_lengths(node.left)
        self.compute_edge_lengths(node.right)

        if not node.left and not node.right:
            node.edge_length = 0
        else:
            delta = 4
            if node.left:
                self.compute_rprofile(node.left, 0, 0)
            if node.right:
                self.compute_lprofile(node.right, 0, 0)
            for i in range(min(node.left.height if node.left else 0, node.right.height if node.right else 0)):
                delta = max(delta, self.gap + 1 + self.rprofile[i] - self.lprofile[i])
            node.edge_length = (delta + 1) // 2 - 1

        node.height = max(node.left.height if node.left else 0, node.right.height if node.right else 0) + node.edge_length + 1

    def print_level(self, node, x, level):
        if not node:
            return
        if level == 0:
            print(' ' * (x - self.print_next - node.lablen // 2), end='')
            print(node.label, end='')
            self.print_next += x - self.print_next - node.lablen // 2 + len(node.label)
        elif node.edge_length >= level:
            if node.left:
                print(' ' * (x - self.print_next - level), end='')
                print('/', end='')
                self.print_next += x - self.print_next - level + 1
            if node.right:
                print(' ' * (x - self.print_next + level), end='')
                print('\\', end='')
                self.print_next += x - self.print_next + level + 1
        else:
            self.print_level(node.left, x - node.edge_length - 1, level - node.edge_length - 1)
            self.print_level(node.right, x + node.edge_length + 1, level - node.edge_length - 1)

def print_ascii_tree(root):
    printer = AsciiTreePrinter()
    printer.print_tree(root)


"""
# Usage

from ascii_tree_printer import print_ascii_tree

# Define your own tree node class without needing to import TreeNodeRaw
class MyTreeNode:
    def __init__(self, val):
        self.val = val
        self.left = None
        self.right = None

# Example usage
root = MyTreeNode(1)
root.left = MyTreeNode(2)
root.right = MyTreeNode(3)
root.left.left = MyTreeNode(4)

print_ascii_tree(root)


"""