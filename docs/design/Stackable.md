# Stackable entities

__Status__: implemented

Stacking handled by a specific Stackable domain.

## Use cases

Combining two stacks: Move op, moving stack1 into stack2. Stack1 destroyed, stack2 grows.

Splitting one stack: Move op of stack1, with extra "amount" attr. Creates a new entity and moves it as a normal entity.

Moving stack: Standard Move op.

When stack receives Delete op it should decrease stack size if possible.

Stackables should not have status properties.

Stack handling is handled by client code. Inventory items should be placed in stacks where possible.
