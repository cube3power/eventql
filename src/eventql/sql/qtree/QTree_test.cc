/**
 * This file is part of the "libcsql" project
 *   Copyright (c) 2015 Paul Asmuth
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <eventql/util/stdtypes.h>
#include <eventql/util/exception.h>
#include <eventql/util/wallclock.h>
#include <eventql/util/test/unittest.h>
#include "eventql/sql/svalue.h"
#include "eventql/sql/runtime/defaultruntime.h"
#include "eventql/sql/qtree/SequentialScanNode.h"
#include "eventql/sql/qtree/ColumnReferenceNode.h"
#include "eventql/sql/qtree/CallExpressionNode.h"
#include "eventql/sql/qtree/LiteralExpressionNode.h"
#include "eventql/sql/qtree/QueryTreeUtil.h"
#include "eventql/sql/qtree/qtree_coder.h"
#include "eventql/sql/CSTableScanProvider.h"

using namespace stx;
using namespace csql;

UNIT_TEST(QTreeTest);

TEST_CASE(QTreeTest, TestExtractEqualsConstraint, [] () {
  auto runtime = Runtime::getDefaultRuntime();
  auto txn = runtime->newTransaction();

  txn->addTableProvider(
      new CSTableScanProvider(
          "testtable",
          "src/eventql/sql/testdata/testtbl.cst"));

  Vector<String> queries;
  queries.push_back("select 1 from testtable where time = 1234;");
  queries.push_back("select 1 from testtable where 1234 = time;");
  queries.push_back("select 1 from testtable where time = 1000 + 234;");
  queries.push_back("select 1 from testtable where 1000 + 234 = time;");
  for (const auto& query : queries) {
    csql::Parser parser;
    parser.parse(query.data(), query.size());

    auto qtree_builder = runtime->queryPlanBuilder();
    auto qtrees = qtree_builder->build(
        txn.get(),
        parser.getStatements(),
        txn->getTableProvider());

    EXPECT_EQ(qtrees.size(), 1);
    auto qtree = qtrees[0];
    EXPECT_TRUE(dynamic_cast<SequentialScanNode*>(qtree.get()) != nullptr);
    auto seqscan = qtree.asInstanceOf<SequentialScanNode>();
    EXPECT_EQ(seqscan->tableName(), "testtable");

    auto constraints = seqscan->constraints();
    EXPECT_EQ(constraints.size(), 1);

    auto constraint = constraints[0];
    EXPECT_EQ(constraint.column_name, "time");
    EXPECT_TRUE(constraint.type == ScanConstraintType::EQUAL_TO);
    EXPECT_EQ(constraint.value.getString(), "1234");
  }
});

TEST_CASE(QTreeTest, TestExtractNotEqualsConstraint, [] () {
  auto runtime = Runtime::getDefaultRuntime();
  auto txn = runtime->newTransaction();

  txn->addTableProvider(
      new CSTableScanProvider(
          "testtable",
          "src/eventql/sql/testdata/testtbl.cst"));

  Vector<String> queries;
  queries.push_back("select 1 from testtable where time != 1234;");
  queries.push_back("select 1 from testtable where time != 1000 + 234;");
  queries.push_back("select 1 from testtable where 1234 != time;");
  queries.push_back("select 1 from testtable where 1000 + 234 != time;");
  for (const auto& query : queries) {
    csql::Parser parser;
    parser.parse(query.data(), query.size());

    auto qtree_builder = runtime->queryPlanBuilder();
    auto qtrees = qtree_builder->build(
        txn.get(),
        parser.getStatements(),
        txn->getTableProvider());

    EXPECT_EQ(qtrees.size(), 1);
    auto qtree = qtrees[0];
    EXPECT_TRUE(dynamic_cast<SequentialScanNode*>(qtree.get()) != nullptr);
    auto seqscan = qtree.asInstanceOf<SequentialScanNode>();
    EXPECT_EQ(seqscan->tableName(), "testtable");

    auto constraints = seqscan->constraints();
    EXPECT_EQ(constraints.size(), 1);

    auto constraint = constraints[0];
    EXPECT_EQ(constraint.column_name, "time");
    EXPECT_TRUE(constraint.type == ScanConstraintType::NOT_EQUAL_TO);
    EXPECT_EQ(constraint.value.getString(), "1234");
  }
});

TEST_CASE(QTreeTest, TestExtractLessThanConstraint, [] () {
  auto runtime = Runtime::getDefaultRuntime();
  auto txn = runtime->newTransaction();

  txn->addTableProvider(
      new CSTableScanProvider(
          "testtable",
          "src/eventql/sql/testdata/testtbl.cst"));

  Vector<String> queries;
  queries.push_back("select 1 from testtable where time < 1234;");
  queries.push_back("select 1 from testtable where time < 1000 + 234;");
  queries.push_back("select 1 from testtable where 1234 > time;");
  queries.push_back("select 1 from testtable where 1000 + 234 > time;");
  for (const auto& query : queries) {
    csql::Parser parser;
    parser.parse(query.data(), query.size());

    auto qtree_builder = runtime->queryPlanBuilder();
    auto qtrees = qtree_builder->build(
        txn.get(),
        parser.getStatements(),
        txn->getTableProvider());

    EXPECT_EQ(qtrees.size(), 1);
    auto qtree = qtrees[0];
    EXPECT_TRUE(dynamic_cast<SequentialScanNode*>(qtree.get()) != nullptr);
    auto seqscan = qtree.asInstanceOf<SequentialScanNode>();
    EXPECT_EQ(seqscan->tableName(), "testtable");

    auto constraints = seqscan->constraints();
    EXPECT_EQ(constraints.size(), 1);

    auto constraint = constraints[0];
    EXPECT_EQ(constraint.column_name, "time");
    EXPECT_TRUE(constraint.type == ScanConstraintType::LESS_THAN);
    EXPECT_EQ(constraint.value.getString(), "1234");
  }
});

TEST_CASE(QTreeTest, TestExtractLessThanOrEqualToConstraint, [] () {
  auto runtime = Runtime::getDefaultRuntime();
  auto txn = runtime->newTransaction();

  txn->addTableProvider(
      new CSTableScanProvider(
          "testtable",
          "src/eventql/sql/testdata/testtbl.cst"));

  Vector<String> queries;
  queries.push_back("select 1 from testtable where time <= 1234;");
  queries.push_back("select 1 from testtable where time <= 1000 + 234;");
  queries.push_back("select 1 from testtable where 1234 >= time;");
  queries.push_back("select 1 from testtable where 1000 + 234 >= time;");
  for (const auto& query : queries) {
    csql::Parser parser;
    parser.parse(query.data(), query.size());

    auto qtree_builder = runtime->queryPlanBuilder();
    auto qtrees = qtree_builder->build(
        txn.get(),
        parser.getStatements(),
        txn->getTableProvider());

    EXPECT_EQ(qtrees.size(), 1);
    auto qtree = qtrees[0];
    EXPECT_TRUE(dynamic_cast<SequentialScanNode*>(qtree.get()) != nullptr);
    auto seqscan = qtree.asInstanceOf<SequentialScanNode>();
    EXPECT_EQ(seqscan->tableName(), "testtable");

    auto constraints = seqscan->constraints();
    EXPECT_EQ(constraints.size(), 1);

    auto constraint = constraints[0];
    EXPECT_EQ(constraint.column_name, "time");
    EXPECT_TRUE(constraint.type == ScanConstraintType::LESS_THAN_OR_EQUAL_TO);
    EXPECT_EQ(constraint.value.getString(), "1234");
  }
});

TEST_CASE(QTreeTest, TestExtractGreaterThanConstraint, [] () {
  auto runtime = Runtime::getDefaultRuntime();
  auto txn = runtime->newTransaction();

  txn->addTableProvider(
      new CSTableScanProvider(
          "testtable",
          "src/eventql/sql/testdata/testtbl.cst"));

  Vector<String> queries;
  queries.push_back("select 1 from testtable where time > 1234;");
  queries.push_back("select 1 from testtable where time > 1000 + 234;");
  queries.push_back("select 1 from testtable where 1234 < time;");
  queries.push_back("select 1 from testtable where 1000 + 234 < time;");
  for (const auto& query : queries) {
    csql::Parser parser;
    parser.parse(query.data(), query.size());

    auto qtree_builder = runtime->queryPlanBuilder();
    auto qtrees = qtree_builder->build(
        txn.get(),
        parser.getStatements(),
        txn->getTableProvider());

    EXPECT_EQ(qtrees.size(), 1);
    auto qtree = qtrees[0];
    EXPECT_TRUE(dynamic_cast<SequentialScanNode*>(qtree.get()) != nullptr);
    auto seqscan = qtree.asInstanceOf<SequentialScanNode>();
    EXPECT_EQ(seqscan->tableName(), "testtable");

    auto constraints = seqscan->constraints();
    EXPECT_EQ(constraints.size(), 1);

    auto constraint = constraints[0];
    EXPECT_EQ(constraint.column_name, "time");
    EXPECT_TRUE(constraint.type == ScanConstraintType::GREATER_THAN);
    EXPECT_EQ(constraint.value.getString(), "1234");
  }
});

TEST_CASE(QTreeTest, TestExtractGreaterThanOrEqualToConstraint, [] () {
  auto runtime = Runtime::getDefaultRuntime();
  auto txn = runtime->newTransaction();

  txn->addTableProvider(
      new CSTableScanProvider(
          "testtable",
          "src/eventql/sql/testdata/testtbl.cst"));

  Vector<String> queries;
  queries.push_back("select 1 from testtable where time >= 1234;");
  queries.push_back("select 1 from testtable where time >= 1000 + 234;");
  queries.push_back("select 1 from testtable where 1234 <= time;");
  queries.push_back("select 1 from testtable where 1000 + 234 <= time;");
  for (const auto& query : queries) {
    csql::Parser parser;
    parser.parse(query.data(), query.size());

    auto qtree_builder = runtime->queryPlanBuilder();
    auto qtrees = qtree_builder->build(
        txn.get(),
        parser.getStatements(),
        txn->getTableProvider());

    EXPECT_EQ(qtrees.size(), 1);
    auto qtree = qtrees[0];
    EXPECT_TRUE(dynamic_cast<SequentialScanNode*>(qtree.get()) != nullptr);
    auto seqscan = qtree.asInstanceOf<SequentialScanNode>();
    EXPECT_EQ(seqscan->tableName(), "testtable");

    auto constraints = seqscan->constraints();
    EXPECT_EQ(constraints.size(), 1);

    auto constraint = constraints[0];
    EXPECT_EQ(constraint.column_name, "time");
    EXPECT_TRUE(constraint.type == ScanConstraintType::GREATER_THAN_OR_EQUAL_TO);
    EXPECT_EQ(constraint.value.getString(), "1234");
  }
});

TEST_CASE(QTreeTest, TestExtractMultipleConstraints, [] () {
  auto runtime = Runtime::getDefaultRuntime();
  auto txn = runtime->newTransaction();

  txn->addTableProvider(
      new CSTableScanProvider(
          "testtable",
          "src/eventql/sql/testdata/testtbl.cst"));

  String query = "select 1 from testtable where 1000 + 200 + 30 + 4 > time AND session_id != 400 + 44 AND time >= 1111 * 6;";

  csql::Parser parser;
  parser.parse(query.data(), query.size());

  auto qtree_builder = runtime->queryPlanBuilder();
  auto qtrees = qtree_builder->build(
      txn.get(),
      parser.getStatements(),
      txn->getTableProvider());

  EXPECT_EQ(qtrees.size(), 1);
  auto qtree = qtrees[0];
  EXPECT_TRUE(dynamic_cast<SequentialScanNode*>(qtree.get()) != nullptr);
  auto seqscan = qtree.asInstanceOf<SequentialScanNode>();
  EXPECT_EQ(seqscan->tableName(), "testtable");

  auto constraints = seqscan->constraints();
  EXPECT_EQ(constraints.size(), 3);

  {
    auto constraint = constraints[0];
    EXPECT_EQ(constraint.column_name, "time");
    EXPECT_TRUE(constraint.type == ScanConstraintType::LESS_THAN);
    EXPECT_EQ(constraint.value.getString(), "1234");
  }

  {
    auto constraint = constraints[1];
    EXPECT_EQ(constraint.column_name, "session_id");
    EXPECT_TRUE(constraint.type == ScanConstraintType::NOT_EQUAL_TO);
    EXPECT_EQ(constraint.value.getString(), "444");
  }

  {
    auto constraint = constraints[2];
    EXPECT_EQ(constraint.column_name, "time");
    EXPECT_TRUE(constraint.type == ScanConstraintType::GREATER_THAN_OR_EQUAL_TO);
    EXPECT_EQ(constraint.value.getString(), "6666");
  }
});

TEST_CASE(QTreeTest, TestSimpleConstantFolding, [] () {
  auto runtime = Runtime::getDefaultRuntime();
  auto txn = runtime->newTransaction();

  txn->addTableProvider(
      new CSTableScanProvider(
          "testtable",
          "src/eventql/sql/testdata/testtbl.cst"));

  String query = "select 1 + 2 + 3 from testtable where time > ucase('fu') + lcase('Bar');";

  csql::Parser parser;
  parser.parse(query.data(), query.size());

  auto qtree_builder = runtime->queryPlanBuilder();
  auto qtrees = qtree_builder->build(
      txn.get(),
      parser.getStatements(),
      txn->getTableProvider());

  EXPECT_EQ(qtrees.size(), 1);
  auto qtree = qtrees[0];
  EXPECT_TRUE(dynamic_cast<SequentialScanNode*>(qtree.get()) != nullptr);
  auto seqscan = qtree.asInstanceOf<SequentialScanNode>();
  EXPECT_EQ(seqscan->tableName(), "testtable");

  auto where_expr = seqscan->whereExpression();
  EXPECT_FALSE(where_expr.isEmpty());
  EXPECT_EQ(where_expr.get()->toSQL(), "gt(`time`,\"FUbar\")");
});

TEST_CASE(QTreeTest, TestPruneConstraints, [] () {
  auto runtime = Runtime::getDefaultRuntime();
  auto txn = runtime->newTransaction();

  txn->addTableProvider(
      new CSTableScanProvider(
          "testtable",
          "src/eventql/sql/testdata/testtbl.cst"));

  String query = "select 1 from testtable where 1000 + 200 + 30 + 4 > time AND session_id != 400 + 44 AND time >= 1111 * 6;";
  csql::Parser parser;
  parser.parse(query.data(), query.size());

  auto qtree_builder = runtime->queryPlanBuilder();
  auto qtrees = qtree_builder->build(
      txn.get(),
      parser.getStatements(),
      txn->getTableProvider());

  EXPECT_EQ(qtrees.size(), 1);
  auto qtree = qtrees[0];
  EXPECT_TRUE(dynamic_cast<SequentialScanNode*>(qtree.get()) != nullptr);
  auto seqscan = qtree.asInstanceOf<SequentialScanNode>();
  auto where_expr = seqscan->whereExpression().get();

  EXPECT_EQ(
      where_expr->toSQL(),
      "logical_and(logical_and(gt(1234,`time`),neq(`session_id`,444)),gte(`time`,6666))");

  {
    ScanConstraint constraint;
    constraint.column_name = "time";
    constraint.type = ScanConstraintType::GREATER_THAN_OR_EQUAL_TO;
    constraint.value = SValue(SValue::IntegerType(6666));
    auto pruned_expr = QueryTreeUtil::removeConstraintFromPredicate(
        where_expr,
        constraint);

    EXPECT_EQ(
        pruned_expr->toSQL(),
        "logical_and(logical_and(gt(1234,`time`),neq(`session_id`,444)),true)");
  }

  {
    ScanConstraint constraint;
    constraint.column_name = "time";
    constraint.type = ScanConstraintType::LESS_THAN;
    constraint.value = SValue(SValue::IntegerType(1234));
    auto pruned_expr = QueryTreeUtil::removeConstraintFromPredicate(
        where_expr,
        constraint);

    EXPECT_EQ(
        pruned_expr->toSQL(),
        "logical_and(logical_and(true,neq(`session_id`,444)),gte(`time`,6666))");
  }
});

TEST_CASE(QTreeTest, TestSerialization, [] () {
  auto runtime = Runtime::getDefaultRuntime();
  auto txn = runtime->newTransaction();

  txn->addTableProvider(
      new CSTableScanProvider(
          "testtable",
          "src/eventql/sql/testdata/testtbl.cst"));

  String query = "select 1 + 2 + 3 from testtable where time > ucase('fu') + lcase('Bar') limit 10;";

  csql::Parser parser;
  parser.parse(query.data(), query.size());

  auto qtree_builder = runtime->queryPlanBuilder();
  auto qtrees = qtree_builder->build(
      txn.get(),
      parser.getStatements(),
      txn->getTableProvider());

  EXPECT_EQ(qtrees.size(), 1);
  auto qtree = qtrees[0];

  QueryTreeCoder coder(txn.get());

  Buffer buf;
  auto buf_os = BufferOutputStream::fromBuffer(&buf);
  coder.encode(qtree, buf_os.get());

  auto buf_is = BufferInputStream::fromBuffer(&buf);
  auto qtree2 = coder.decode(buf_is.get());

  EXPECT_EQ(qtree->toString(), qtree2->toString());
});

TEST_CASE(QTreeTest, TestSerialization2, [] () {
  auto runtime = Runtime::getDefaultRuntime();
  auto txn = runtime->newTransaction();

  txn->addTableProvider(
      new CSTableScanProvider(
          "testtable",
          "src/eventql/sql/testdata/testtbl.cst"));

  String query =
        "SELECT  \
          t1.time, t2.time, t3.time, t1.x, t2.x, t1.x + t2.x, t1.x * 3 = t3.x, x1, x2, x3 \
        FROM  \
          (select TRUNCATE(time / 1000000) as time, count(1) as x, 123 as x1 from testtable group by TRUNCATE(time / 1200000000)) t1, \
          (select TRUNCATE(time / 1000000) as time, sum(2) as x, 456 as x2 from testtable group by TRUNCATE(time / 1200000000)) AS t2,  \
          (select TRUNCATE(time / 1000000) as time, sum(3) as x, 789 as x3 from testtable group by TRUNCATE(time / 1200000000)) AS t3   \
        ORDER BY \
          t1.time desc;";

  csql::Parser parser;
  parser.parse(query.data(), query.size());

  auto qtree_builder = runtime->queryPlanBuilder();
  auto qtrees = qtree_builder->build(
      txn.get(),
      parser.getStatements(),
      txn->getTableProvider());

  EXPECT_EQ(qtrees.size(), 1);
  auto qtree = qtrees[0];

  QueryTreeCoder coder(txn.get());

  Buffer buf;
  auto buf_os = BufferOutputStream::fromBuffer(&buf);
  coder.encode(qtree, buf_os.get());

  auto buf_is = BufferInputStream::fromBuffer(&buf);
  auto qtree2 = coder.decode(buf_is.get());

  EXPECT_EQ(qtree->toString(), qtree2->toString());
});
