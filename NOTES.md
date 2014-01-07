# Various notes about Darm

## CBZ and CBNZ

In the documentation the first operand of the *CBZ* and *CBNZ* instructions
has been defined as *Rn*. However, as the *label* operand refers to a memory
address, for which darm sets *Rn* to *PC*, darm exposes the first argument as
*Rm*. Other instructions use either *Rd* or *Rt* as operand, thus their label
operand does not suffer from this minor design flaw.
