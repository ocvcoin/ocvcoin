# Libraries

| Name                     | Description |
|--------------------------|-------------|
| *libocvcoin_cli*         | RPC client functionality used by *ocvcoin-cli* executable |
| *libocvcoin_common*      | Home for common functionality shared by different executables and libraries. Similar to *libocvcoin_util*, but higher-level (see [Dependencies](#dependencies)). |
| *libocvcoin_consensus*   | Stable, backwards-compatible consensus functionality used by *libocvcoin_node* and *libocvcoin_wallet* and also exposed as a [shared library](../shared-libraries.md). |
| *libocvcoinconsensus*    | Shared library build of static *libocvcoin_consensus* library |
| *libocvcoin_kernel*      | Consensus engine and support library used for validation by *libocvcoin_node* and also exposed as a [shared library](../shared-libraries.md). |
| *libocvcoinqt*           | GUI functionality used by *ocvcoin-qt* and *ocvcoin-gui* executables |
| *libocvcoin_ipc*         | IPC functionality used by *ocvcoin-node*, *ocvcoin-wallet*, *ocvcoin-gui* executables to communicate when [`--enable-multiprocess`](multiprocess.md) is used. |
| *libocvcoin_node*        | P2P and RPC server functionality used by *ocvcoind* and *ocvcoin-qt* executables. |
| *libocvcoin_util*        | Home for common functionality shared by different executables and libraries. Similar to *libocvcoin_common*, but lower-level (see [Dependencies](#dependencies)). |
| *libocvcoin_wallet*      | Wallet functionality used by *ocvcoind* and *ocvcoin-wallet* executables. |
| *libocvcoin_wallet_tool* | Lower-level wallet functionality used by *ocvcoin-wallet* executable. |
| *libocvcoin_zmq*         | [ZeroMQ](../zmq.md) functionality used by *ocvcoind* and *ocvcoin-qt* executables. |

## Conventions

- Most libraries are internal libraries and have APIs which are completely unstable! There are few or no restrictions on backwards compatibility or rules about external dependencies. Exceptions are *libocvcoin_consensus* and *libocvcoin_kernel* which have external interfaces documented at [../shared-libraries.md](../shared-libraries.md).

- Generally each library should have a corresponding source directory and namespace. Source code organization is a work in progress, so it is true that some namespaces are applied inconsistently, and if you look at [`libocvcoin_*_SOURCES`](../../src/Makefile.am) lists you can see that many libraries pull in files from outside their source directory. But when working with libraries, it is good to follow a consistent pattern like:

  - *libocvcoin_node* code lives in `src/node/` in the `node::` namespace
  - *libocvcoin_wallet* code lives in `src/wallet/` in the `wallet::` namespace
  - *libocvcoin_ipc* code lives in `src/ipc/` in the `ipc::` namespace
  - *libocvcoin_util* code lives in `src/util/` in the `util::` namespace
  - *libocvcoin_consensus* code lives in `src/consensus/` in the `Consensus::` namespace

## Dependencies

- Libraries should minimize what other libraries they depend on, and only reference symbols following the arrows shown in the dependency graph below:

<table><tr><td>

```mermaid

%%{ init : { "flowchart" : { "curve" : "basis" }}}%%

graph TD;

ocvcoin-cli[ocvcoin-cli]-->libocvcoin_cli;

ocvcoind[ocvcoind]-->libocvcoin_node;
ocvcoind[ocvcoind]-->libocvcoin_wallet;

ocvcoin-qt[ocvcoin-qt]-->libocvcoin_node;
ocvcoin-qt[ocvcoin-qt]-->libocvcoinqt;
ocvcoin-qt[ocvcoin-qt]-->libocvcoin_wallet;

ocvcoin-wallet[ocvcoin-wallet]-->libocvcoin_wallet;
ocvcoin-wallet[ocvcoin-wallet]-->libocvcoin_wallet_tool;

libocvcoin_cli-->libocvcoin_util;
libocvcoin_cli-->libocvcoin_common;

libocvcoin_common-->libocvcoin_consensus;
libocvcoin_common-->libocvcoin_util;

libocvcoin_kernel-->libocvcoin_consensus;
libocvcoin_kernel-->libocvcoin_util;

libocvcoin_node-->libocvcoin_consensus;
libocvcoin_node-->libocvcoin_kernel;
libocvcoin_node-->libocvcoin_common;
libocvcoin_node-->libocvcoin_util;

libocvcoinqt-->libocvcoin_common;
libocvcoinqt-->libocvcoin_util;

libocvcoin_wallet-->libocvcoin_common;
libocvcoin_wallet-->libocvcoin_util;

libocvcoin_wallet_tool-->libocvcoin_wallet;
libocvcoin_wallet_tool-->libocvcoin_util;

classDef bold stroke-width:2px, font-weight:bold, font-size: smaller;
class ocvcoin-qt,ocvcoind,ocvcoin-cli,ocvcoin-wallet bold
```
</td></tr><tr><td>

**Dependency graph**. Arrows show linker symbol dependencies. *Consensus* lib depends on nothing. *Util* lib is depended on by everything. *Kernel* lib depends only on consensus and util.

</td></tr></table>

- The graph shows what _linker symbols_ (functions and variables) from each library other libraries can call and reference directly, but it is not a call graph. For example, there is no arrow connecting *libocvcoin_wallet* and *libocvcoin_node* libraries, because these libraries are intended to be modular and not depend on each other's internal implementation details. But wallet code is still able to call node code indirectly through the `interfaces::Chain` abstract class in [`interfaces/chain.h`](../../src/interfaces/chain.h) and node code calls wallet code through the `interfaces::ChainClient` and `interfaces::Chain::Notifications` abstract classes in the same file. In general, defining abstract classes in [`src/interfaces/`](../../src/interfaces/) can be a convenient way of avoiding unwanted direct dependencies or circular dependencies between libraries.

- *libocvcoin_consensus* should be a standalone dependency that any library can depend on, and it should not depend on any other libraries itself.

- *libocvcoin_util* should also be a standalone dependency that any library can depend on, and it should not depend on other internal libraries.

- *libocvcoin_common* should serve a similar function as *libocvcoin_util* and be a place for miscellaneous code used by various daemon, GUI, and CLI applications and libraries to live. It should not depend on anything other than *libocvcoin_util* and *libocvcoin_consensus*. The boundary between _util_ and _common_ is a little fuzzy but historically _util_ has been used for more generic, lower-level things like parsing hex, and _common_ has been used for ocvcoin-specific, higher-level things like parsing base58. The difference between util and common is mostly important because *libocvcoin_kernel* is not supposed to depend on *libocvcoin_common*, only *libocvcoin_util*. In general, if it is ever unclear whether it is better to add code to *util* or *common*, it is probably better to add it to *common* unless it is very generically useful or useful particularly to include in the kernel.


- *libocvcoin_kernel* should only depend on *libocvcoin_util* and *libocvcoin_consensus*.

- The only thing that should depend on *libocvcoin_kernel* internally should be *libocvcoin_node*. GUI and wallet libraries *libocvcoinqt* and *libocvcoin_wallet* in particular should not depend on *libocvcoin_kernel* and the unneeded functionality it would pull in, like block validation. To the extent that GUI and wallet code need scripting and signing functionality, they should be get able it from *libocvcoin_consensus*, *libocvcoin_common*, and *libocvcoin_util*, instead of *libocvcoin_kernel*.

- GUI, node, and wallet code internal implementations should all be independent of each other, and the *libocvcoinqt*, *libocvcoin_node*, *libocvcoin_wallet* libraries should never reference each other's symbols. They should only call each other through [`src/interfaces/`](`../../src/interfaces/`) abstract interfaces.

## Work in progress

- Validation code is moving from *libocvcoin_node* to *libocvcoin_kernel* as part of [The libocvcoinkernel Project #24303](https://github.com/ocvcoin/ocvcoin/issues/24303)
- Source code organization is discussed in general in [Library source code organization #15732](https://github.com/ocvcoin/ocvcoin/issues/15732)
