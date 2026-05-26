#include "factDB/infra/iu.hpp"
#include "factDB/newftree/FIterator.hpp"
#include "factDB/newftree/FTree.hpp"
#include "factDB/schemac/Type.hpp"
#include <gtest/gtest.h>
// ---------------------------------------------------------------------------------------------------
using namespace std;
using namespace factDB;
// ---------------------------------------------------------------------------------------------------
FTree generate_paper_tree(IU& iuA, IU& iuB, IU& iuC, IU& iuD) {
   FNodeOwning rootNode;
   rootNode.addIUs(iuA);
   [[maybe_unused]] auto& bChild = rootNode.addChild().addIUs(iuB);
   [[maybe_unused]] auto& cChild = bChild.addChild().addIUs(iuC);
   [[maybe_unused]] auto& dChild = rootNode.addChild().addIUs(iuD);

   return FTree::fromNodes(std::move(rootNode));
}
// ---------------------------------------------------------------------------------------------------
TEST(TestFIterator, Basic) {
   IU iuA("A", schemac::Type::Integer());
   IU iuB("B", schemac::Type::Integer());
   IU iuC("C", schemac::Type::Integer());
   IU iuD("D", schemac::Type::Integer());

   auto tree = generate_paper_tree(iuA, iuB, iuC, iuD);

   ASSERT_EQ(tree.getRootNode().getIUs().size(), 1);
   ASSERT_TRUE(tree.getRootNode().getIUs().contains(iuA));

   ASSERT_EQ(tree.getRootNode().getChildren().size(), 2);
   auto& nodeB = tree.getRootNode().getChildren().front();
   auto& nodeD = tree.getRootNode().getChildren().back();

   ASSERT_EQ(nodeB->getIUs().size(), 1);
   ASSERT_TRUE(nodeB->getIUs().contains(iuB));
   ASSERT_EQ(nodeB->getChildren().size(), 1);
   auto& nodeC = nodeB->getChildren().front();

   ASSERT_EQ(nodeC->getChildren().size(), 0);
   ASSERT_EQ(nodeC->getIUs().size(), 1);
   ASSERT_TRUE(nodeC->getIUs().contains(iuC));

   ASSERT_EQ(nodeD->getChildren().size(), 0);
   ASSERT_EQ(nodeD->getIUs().size(), 1);
   ASSERT_TRUE(nodeD->getIUs().contains(iuD));
}
// ---------------------------------------------------------------------------------------------------
TEST(TestFIterator, Equality) {
   IU iuA("A", schemac::Type::Integer());
   IU iuB("B", schemac::Type::Integer());
   IU iuC("C", schemac::Type::Integer());
   IU iuD("D", schemac::Type::Integer());

   auto tree1 = generate_paper_tree(iuA, iuB, iuC, iuD);
   auto tree2 = generate_paper_tree(iuA, iuB, iuC, iuD);

   ASSERT_TRUE(tree1 == tree1);
   ASSERT_TRUE(tree2 == tree2);

   ASSERT_TRUE(tree1 == tree2);

   {
      FNodeOwning rootNode3;
      rootNode3.addIUs(iuA);
      [[maybe_unused]] auto& bChild = rootNode3.addChild().addIUs(iuB);
      [[maybe_unused]] auto& cChild = bChild.addChild().addIUs(iuC);
      FTree tree3 = FTree::fromNodes(std::move(rootNode3));

      ASSERT_FALSE(tree1 == tree3);
   }

   {
      FNodeOwning rootNode;
      rootNode.addIUs(iuA);
      [[maybe_unused]] auto& bChild = rootNode.addChild().addIUs(iuB);
      [[maybe_unused]] auto& cChild = bChild.addChild().addIUs(iuC);
      [[maybe_unused]] auto& dChild = rootNode.addChild().addIUs({iuD, iuD});
      FTree tree3 = FTree::fromNodes(std::move(rootNode));

      ASSERT_FALSE(tree1 == tree3);
   }

   { // swap children insertion
      FNodeOwning rootNode;
      rootNode.addIUs(iuA);
      [[maybe_unused]] auto& bChild = rootNode.addChild().addIUs(iuB);
      [[maybe_unused]] auto& dChild = rootNode.addChild().addIUs(iuD);
      [[maybe_unused]] auto& cChild = bChild.addChild().addIUs(iuC);
      FTree tree3 = FTree::fromNodes(std::move(rootNode));

      ASSERT_TRUE(tree1 == tree3);
   }
}
// ---------------------------------------------------------------------------------------------------
TEST(TestFIterator, RequiredNodes) {
   IU iuA("A", schemac::Type::Integer());
   IU iuB("B", schemac::Type::Integer());
   IU iuC("C", schemac::Type::Integer());
   IU iuD("D", schemac::Type::Integer());

   FTree tree = generate_paper_tree(iuA, iuB, iuC, iuD);

   auto nodeAid = tree.getRootNode().getNodeId();
   auto nodeBid = tree.getRootNode().getChildren().front()->getNodeId();
   auto nodeCid = tree.getRootNode().getChildren().front()->getChildren().front()->getNodeId();
   auto nodeDid = tree.getRootNode().getChildren().back()->getNodeId();

   // std::cout << "nodeA: " << nodeAid << " , nodeB: " << nodeBid << ", nodeC: " << nodeCid << ", nodeD: " << nodeDid << std::endl;

   auto checkIterator = [&](OrderedIUSet iteratedIUs, bool nodeARequired, bool nodeBRequired, bool nodeCRequired, bool nodeDRequired) {
      auto iter = tree.createIterator(iteratedIUs);
      auto requiredNodeIDs = iter.getRequiredNodeIDs();
      size_t countRequiredNodes = nodeARequired + nodeBRequired + nodeCRequired + nodeDRequired;
      ASSERT_EQ(requiredNodeIDs.size(), countRequiredNodes);
      ASSERT_EQ(requiredNodeIDs.contains(nodeAid), nodeARequired);
      ASSERT_EQ(requiredNodeIDs.contains(nodeBid), nodeBRequired);
      ASSERT_EQ(requiredNodeIDs.contains(nodeCid), nodeCRequired);
      ASSERT_EQ(requiredNodeIDs.contains(nodeDid), nodeDRequired);
   };

   // all nodes
   checkIterator({iuA, iuB, iuC, iuD}, true, true, true, true);
   // only leafs
   checkIterator({iuC, iuD}, true, true, true, true);
   // only left subtree
   checkIterator({iuA, iuB, iuC}, true, true, true, false);
   checkIterator({iuB, iuC}, true, true, true, false);
   checkIterator({iuC}, true, true, true, false);
   // only right subtree
   checkIterator({iuD}, true, false, false, true);

   // only root
   checkIterator({iuA}, true, false, false, false);
   // only B node
   checkIterator({iuB}, true, true, false, false);
   // iterate none
   checkIterator({}, false, false, false, false);
}
// ---------------------------------------------------------------------------------------------------
TEST(TestFIterator, MergeTrees) {
   IU iuA("A", schemac::Type::Integer());
   IU iuB("B", schemac::Type::Integer());
   IU iuC("C", schemac::Type::Integer());
   IU iuD("D", schemac::Type::Integer());
   IU iuR("R", schemac::Type::Integer());

   FTree treeLeft = generate_paper_tree(iuA, iuB, iuC, iuD);

   FNodeOwning rNode;
   rNode.addIUs(iuR);
   FTree treeRight = FTree::fromNodes(std::move(rNode));

   auto genResTreeTI = [&]() {
      FNodeOwning rootNode;
      rootNode.addIUs(iuR);
      [[maybe_unused]] auto& nodeA = rootNode.addChild().addIUs(iuA);
      [[maybe_unused]] auto& bChild = nodeA.addChild().addIUs(iuB);
      [[maybe_unused]] auto& cChild = bChild.addChild().addIUs(iuC);
      [[maybe_unused]] auto& dChild = nodeA.addChild().addIUs(iuD);
      return FTree::fromNodes(std::move(rootNode));
   };

   { // top insert
      auto resTree = genResTreeTI();
      auto iterLeft = treeLeft.createIterator(iuA);
      auto iterRight = treeRight.createIterator(iuR);
      FTree res;
      FTree::mergeTrees(res, iterLeft, iterRight, factDB::algebra::JoinMode::TopInsert);
      ASSERT_TRUE(res == resTree);
   }
   { // top insert
      auto resTree = genResTreeTI();
      auto iterLeft = treeLeft.createIterator(iuB);
      auto iterRight = treeRight.createIterator(iuR);
      FTree res;
      FTree::mergeTrees(res, iterLeft, iterRight, factDB::algebra::JoinMode::TopInsert);
      ASSERT_TRUE(res == resTree);
   }
   { // top insert
      auto resTree = genResTreeTI();
      auto iterLeft = treeLeft.createIterator(iuC);
      auto iterRight = treeRight.createIterator(iuR);
      FTree res;
      FTree::mergeTrees(res, iterLeft, iterRight, factDB::algebra::JoinMode::TopInsert);
      ASSERT_TRUE(res == resTree);
   }
   { // top insert
      auto resTree = genResTreeTI();
      auto iterLeft = treeLeft.createIterator(iuD);
      auto iterRight = treeRight.createIterator(iuR);
      FTree res;
      FTree::mergeTrees(res, iterLeft, iterRight, factDB::algebra::JoinMode::TopInsert);
      ASSERT_TRUE(res == resTree);
   }
   { // top insert, multi node
      auto resTree = genResTreeTI();
      auto iterLeft = treeLeft.createIterator({iuD, iuA});
      auto iterRight = treeRight.createIterator(iuR);
      FTree res;
      FTree::mergeTrees(res, iterLeft, iterRight, factDB::algebra::JoinMode::TopInsert);
      ASSERT_TRUE(res == resTree);
   }
   { // top insert, multi node
      auto resTree = genResTreeTI();
      auto iterLeft = treeLeft.createIterator({iuC, iuB});
      auto iterRight = treeRight.createIterator(iuR);
      FTree res;
      FTree::mergeTrees(res, iterLeft, iterRight, factDB::algebra::JoinMode::TopInsert);
      ASSERT_TRUE(res == resTree);
   }
   { // top insert, merge required
      auto iterLeft = treeLeft.createIterator({iuC, iuD});
      auto iterRight = treeRight.createIterator(iuR);

      FNodeOwning rootNode;
      [[maybe_unused]] auto& rChild = rootNode.addIUs(iuR);
      [[maybe_unused]] auto& aChild = rChild.addChild().addIUs(iuA);
      [[maybe_unused]] auto& bChild = aChild.addChild().addIUs({iuB, iuD});
      [[maybe_unused]] auto& cChild = bChild.addChild().addIUs(iuC);
      auto resTree = FTree::fromNodes(std::move(rootNode));

      FTree res;
      FTree::mergeTrees(res, iterLeft, iterRight, factDB::algebra::JoinMode::TopInsert);
      ASSERT_TRUE(res == resTree);
   }

   { // bottom insert
      FNodeOwning rootNode;
      [[maybe_unused]] auto& aChild = rootNode.addIUs(iuA);
      [[maybe_unused]] auto& bChild = aChild.addChild().addIUs(iuB);
      [[maybe_unused]] auto& cChild = bChild.addChild().addIUs(iuC);
      [[maybe_unused]] auto& dChild = aChild.addChild().addIUs(iuD);
      [[maybe_unused]] auto& rChild = aChild.addChild().addIUs(iuR);
      auto resTree = FTree::fromNodes(std::move(rootNode));

      auto iterLeft = treeLeft.createIterator(iuA);
      auto iterRight = treeRight.createIterator(iuR);
      FTree res;
      FTree::mergeTrees(res, iterLeft, iterRight, factDB::algebra::JoinMode::BottomInsert);
      ASSERT_TRUE(res == resTree);
   }

   { // bottom insert
      FNodeOwning rootNode;
      [[maybe_unused]] auto& aChild = rootNode.addIUs(iuA);
      [[maybe_unused]] auto& bChild = aChild.addChild().addIUs(iuB);
      [[maybe_unused]] auto& cChild = bChild.addChild().addIUs(iuC);
      [[maybe_unused]] auto& dChild = aChild.addChild().addIUs(iuD);
      [[maybe_unused]] auto& rChild = bChild.addChild().addIUs(iuR);
      auto resTree = FTree::fromNodes(std::move(rootNode));

      auto iterLeft = treeLeft.createIterator(iuB);
      auto iterRight = treeRight.createIterator(iuR);
      FTree res;
      FTree::mergeTrees(res, iterLeft, iterRight, factDB::algebra::JoinMode::BottomInsert);
      ASSERT_TRUE(res == resTree);
   }

   { // bottom insert
      FNodeOwning rootNode;
      [[maybe_unused]] auto& aChild = rootNode.addIUs(iuA);
      [[maybe_unused]] auto& bChild = aChild.addChild().addIUs(iuB);
      [[maybe_unused]] auto& cChild = bChild.addChild().addIUs(iuC);
      [[maybe_unused]] auto& dChild = aChild.addChild().addIUs(iuD);
      [[maybe_unused]] auto& rChild = cChild.addChild().addIUs(iuR);
      auto resTree = FTree::fromNodes(std::move(rootNode));

      auto iterLeft = treeLeft.createIterator(iuC);
      auto iterRight = treeRight.createIterator(iuR);
      FTree res;
      FTree::mergeTrees(res, iterLeft, iterRight, factDB::algebra::JoinMode::BottomInsert);

      ASSERT_TRUE(res == resTree);
   }

   { // bottom insert
      FNodeOwning rootNode;
      [[maybe_unused]] auto& aChild = rootNode.addIUs(iuA);
      [[maybe_unused]] auto& bChild = aChild.addChild().addIUs(iuB);
      [[maybe_unused]] auto& cChild = bChild.addChild().addIUs(iuC);
      [[maybe_unused]] auto& dChild = aChild.addChild().addIUs(iuD);
      [[maybe_unused]] auto& rChild = dChild.addChild().addIUs(iuR);
      auto resTree = FTree::fromNodes(std::move(rootNode));

      auto iterLeft = treeLeft.createIterator(iuD);
      auto iterRight = treeRight.createIterator(iuR);
      FTree res;
      FTree::mergeTrees(res, iterLeft, iterRight, factDB::algebra::JoinMode::BottomInsert);

      ASSERT_TRUE(res == resTree);
   }

   { // bottom insert, 2 nodes
      FNodeOwning rootNode;
      [[maybe_unused]] auto& aChild = rootNode.addIUs(iuA);
      [[maybe_unused]] auto& bChild = aChild.addChild().addIUs(iuB);
      [[maybe_unused]] auto& cChild = bChild.addChild().addIUs(iuC);
      [[maybe_unused]] auto& dChild = aChild.addChild().addIUs(iuD);
      [[maybe_unused]] auto& rChild = dChild.addChild().addIUs(iuR);
      auto resTree = FTree::fromNodes(std::move(rootNode));

      auto iterLeft = treeLeft.createIterator({iuD, iuA});
      auto iterRight = treeRight.createIterator(iuR);
      FTree res;
      FTree::mergeTrees(res, iterLeft, iterRight, factDB::algebra::JoinMode::BottomInsert);

      ASSERT_TRUE(res == resTree);
   }

   { // bottom insert, 2 nodes
      FNodeOwning rootNode;
      [[maybe_unused]] auto& aChild = rootNode.addIUs(iuA);
      [[maybe_unused]] auto& bChild = aChild.addChild().addIUs(iuB);
      [[maybe_unused]] auto& cChild = bChild.addChild().addIUs(iuC);
      [[maybe_unused]] auto& dChild = aChild.addChild().addIUs(iuD);
      [[maybe_unused]] auto& rChild = cChild.addChild().addIUs(iuR);
      auto resTree = FTree::fromNodes(std::move(rootNode));

      auto iterLeft = treeLeft.createIterator({iuB, iuC});
      auto iterRight = treeRight.createIterator(iuR);
      FTree res;
      FTree::mergeTrees(res, iterLeft, iterRight, factDB::algebra::JoinMode::BottomInsert);

      ASSERT_TRUE(res == resTree);
   }

   { // bottom insert, 2 nodes
      FNodeOwning rootNode;
      [[maybe_unused]] auto& aChild = rootNode.addIUs(iuA);
      [[maybe_unused]] auto& bChild = aChild.addChild().addIUs({iuB, iuD});
      [[maybe_unused]] auto& cChild = bChild.addChild().addIUs(iuC);
      [[maybe_unused]] auto& rChild = cChild.addChild().addIUs(iuR);
      auto resTree = FTree::fromNodes(std::move(rootNode));

      auto iterLeft = treeLeft.createIterator({iuD, iuC});
      auto iterRight = treeRight.createIterator(iuR);
      FTree res;
      FTree::mergeTrees(res, iterLeft, iterRight, factDB::algebra::JoinMode::BottomInsert);

      ASSERT_TRUE(res == resTree);
   }
}
// ---------------------------------------------------------------------------------------------------
TEST(TestFIterator, MergeComposedTrees) {
   IU iuA("A", schemac::Type::Integer());
   IU iuB("B", schemac::Type::Integer());
   IU iuC("C", schemac::Type::Integer());
   IU iuD("D", schemac::Type::Integer());
   IU iuR("R", schemac::Type::Integer());

   FNodeOwning rootNodeLeft;
   rootNodeLeft.addIUs(iuR);
   auto leftTree = FTree::fromNodes(std::move(rootNodeLeft));

   FNodeOwning rootNodeRight;
   {
      [[maybe_unused]] auto& aChild = rootNodeRight.addIUs(iuA);
      [[maybe_unused]] auto& bChild = aChild.addChild().addIUs(iuB);
      [[maybe_unused]] auto& cChild = aChild.addChild().addIUs(iuC);
   }
   auto rightTree = FTree::fromNodes(std::move(rootNodeRight));
   rightTree.setComposedTree();

   FNodeOwning rootNodeAfter;
   {
      [[maybe_unused]] auto& aChild = rootNodeRight.addIUs(iuA);
      [[maybe_unused]] auto& bChild = aChild.addChild().addIUs(iuB);
      [[maybe_unused]] auto& cChild = aChild.addChild().addIUs(iuC);
      [[maybe_unused]] auto& rChild = aChild.addChild().addIUs(iuR);
   }
   auto afterTree = FTree::fromNodes(std::move(rootNodeRight));

   auto iterLeft = leftTree.createIterator(iuR);
   auto iterRight = rightTree.createIterator(iuA);
   FTree res;
   FTree::mergeTrees(res, iterLeft, iterRight, factDB::algebra::JoinMode::TopInsert);

   ASSERT_EQ(afterTree, res);
}
// ---------------------------------------------------------------------------------------------------
TEST(TestFIterator, UpperTreeNodes) {
   IU iuA("A", schemac::Type::Integer());
   IU iuB("B", schemac::Type::Integer());
   IU iuC("C", schemac::Type::Integer());
   IU iuD("D", schemac::Type::Integer());
   IU iuR("R", schemac::Type::Integer());

   FNodeOwning rootNodeLeft;
   rootNodeLeft.addIUs(iuR);
   auto leftTree = FTree::fromNodes(std::move(rootNodeLeft));

   FNodeOwning rootNodeRight;
   {
      [[maybe_unused]] auto& aChild = rootNodeRight.addIUs(iuA);
      [[maybe_unused]] auto& bChild = aChild.addChild().addIUs(iuB);
      [[maybe_unused]] auto& cChild = aChild.addChild().addIUs(iuC);
      [[maybe_unused]] auto& dChild = cChild.addChild().addIUs(iuD);
   }
   auto rightTree = FTree::fromNodes(std::move(rootNodeRight));

   FNodeOwning rootNodeAfter;
   auto iterLeft = leftTree.createIterator(iuR);
   auto iterRight = rightTree.createIterator(iuC);
   FTree res;
   FTree::mergeTrees(res, iterLeft, iterRight, factDB::algebra::JoinMode::TopInsert);

   std::unordered_set<size_t> upperNodes;
   IUSet upperIUs = {iuA, iuB, iuC, iuD};
   std::stack<const FNode*> todos;
   todos.emplace(&res.getRootNode());
   while (!todos.empty()) {
      const auto& cur = *todos.top();
      todos.pop();
      assert(cur.getIUs().size() == 1);
      if (upperIUs.contains(cur.getIUs().front()))
         upperNodes.insert(cur.getNodeId());
      for (auto& c : cur.getChildren())
         todos.push(c.get());
   }

   ASSERT_FALSE(upperNodes.empty());
   ASSERT_EQ(upperNodes.size(), 4);
   ASSERT_EQ(upperNodes, res.collectUpperTreeNodes());
}
// ---------------------------------------------------------------------------------------------------