#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/Config.hpp"
#include "factDB/infra/IUSet.hpp"
#include "factDB/queryc/writer/FWUtil.hpp"
#include <list>
#include <memory>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
class FNodeOwning;
class FIterator;
// ---------------------------------------------------------------------------------------------------
class FNode {
   friend class FNodeOwning;
   friend class FNodeReferencing;

   public:
   using ChildrenList = std::list<std::unique_ptr<FNode>>;
   using NodeIdType = std::size_t;

   enum FactorizedNodeType { Reference,
                             Owning };

   FactorizedNodeType type;

   explicit FNode(FactorizedNodeType t) noexcept : type(t){};
   virtual ~FNode();

   bool operator==(const FNode& other) const;
   void print(FileWriter& out = fw::silentCout, bool printNodeId = true, FWContainer subgraphName = fw::nop()) const;

   virtual FNode& addToMergedNode(FNode& child) = 0;
   virtual void addChild(FNode& child, bool referencing = true) = 0;
   virtual FNode& addChild() = 0;

   virtual FNode& addIUs(const OrderedIUSet& insertedIUs) = 0;
   virtual bool containsIU(const IU& iu) const = 0;
   virtual bool containsIU(const IU* iu) const { return containsIU(*iu); }
   virtual NodeIdType getNodeId() const = 0;
   virtual FNode* getMergedNode() const = 0;

   virtual bool needsLock() const = 0;
   bool isLeaf() const { return getChildren().empty(); }

   [[nodiscard]] virtual ChildrenList& getChildren() = 0;
   [[nodiscard]] virtual const ChildrenList& getChildren() const = 0;
   [[nodiscard]] virtual const OrderedIUSet& getIUs() const = 0;

   [[nodiscard]] virtual bool isRequired(const FIterator& iterator) const = 0;
   [[nodiscard]] bool isRequiredByReferenceIterator(const FIterator& iterator) const;

   virtual FNodeOwning& getOwningNode() = 0;
   virtual const FNodeOwning& getOwningNode() const = 0;

   static void resetFNodeId();
};
// ---------------------------------------------------------------------------------------------------
class FNodeOwning : public FNode {
   friend class FNodeReferencing;

   // the id of the node
   NodeIdType nodeID;
   // the IUs held in this node
   OrderedIUSet ius = {};
   // the children of this node
   ChildrenList children = {};
   // the set of dependent IUs, i.e. IUs in the child nodes
   IUSet dependentIUs = {};

   // has the list to be lockable?
   bool isNeedLock = false; // todo set?
   // reference to the merged child node, if exist
   FNode* mergedChild = nullptr;

   public:
   FNodeOwning() noexcept;
   FNodeOwning(FNodeOwning&&) = default;
   ~FNodeOwning() override = default;

   bool operator==(const FNodeOwning& other) const;

   FNodeOwning& getOwningNode() override { return *this; }
   const FNodeOwning& getOwningNode() const override { return *this; }
   FNode& addToMergedNode(FNode& child) override;
   void addChild(FNode& child, bool referencing = true) override;
   FNode& addChild() override;

   FNode& addIUs(const factDB::OrderedIUSet& insertedIUs) override;
   bool containsIU(const IU& iu) const override { return ius.contains(iu); }
   NodeIdType getNodeId() const override { return nodeID; }
   FNode* getMergedNode() const override { return mergedChild; }

   bool needsLock() const override { return isNeedLock; }
   void setRequiresLock() { isNeedLock = true; }

   [[nodiscard]] ChildrenList& getChildren() override { return children; }
   [[nodiscard]] const ChildrenList& getChildren() const override { return children; }
   [[nodiscard]] const OrderedIUSet& getIUs() const override { return ius; }
   [[nodiscard]] bool isRequired(const FIterator& iterator) const override;
};
// ---------------------------------------------------------------------------------------------------
class FNodeReferencing : public FNode {
   FNodeOwning& referencedNode;

   public:
   FNodeReferencing(FNode& ref) noexcept : FNode(Reference), referencedNode(ref.getOwningNode()) {}
   ~FNodeReferencing() override = default;

   FNodeOwning& getOwningNode() override { return referencedNode; }
   const FNodeOwning& getOwningNode() const override { return referencedNode; }

   FNode& addToMergedNode(FNode&) override { unreachable(); };
   void addChild(FNode&, bool) override { unreachable(); }
   FNode& addChild() override { unreachable(); }

   FNode& addIUs(const factDB::OrderedIUSet& insertedIUs) override {
      referencedNode.addIUs(insertedIUs);
      return *this;
   }
   bool containsIU(const IU& iu) const override { return referencedNode.containsIU(iu); }
   NodeIdType getNodeId() const override { return referencedNode.getNodeId(); }
   FNode* getMergedNode() const override { return referencedNode.getMergedNode(); }
   bool needsLock() const override { return referencedNode.needsLock(); }

   [[nodiscard]] ChildrenList& getChildren() override { return referencedNode.getChildren(); }
   [[nodiscard]] const ChildrenList& getChildren() const override { return referencedNode.getChildren(); }
   [[nodiscard]] const OrderedIUSet& getIUs() const override { return referencedNode.getIUs(); }
   [[nodiscard]] bool isRequired(const FIterator& iterator) const override { return referencedNode.isRequired(iterator); }
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------