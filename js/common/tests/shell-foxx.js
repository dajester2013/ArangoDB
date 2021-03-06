require("internal").flushModuleCache();

var jsunity = require("jsunity"),
  FoxxController = require("org/arangodb/foxx").Controller,
  db = require("org/arangodb").db,
  fakeContext,
  stub_and_mock = require("org/arangodb/stub_and_mock"),
  stub = stub_and_mock.stub,
  allow = stub_and_mock.allow,
  expect = stub_and_mock.expect,
  mockConstructor = stub_and_mock.mockConstructor;

fakeContext = {
  prefix: "",
  foxxes: [],
  comments: [],
  clearComments: function () {},
  comment: function () {},
  collectionName: function () {}
};

function CreateFoxxControllerSpec () {
  return {
    testCreationWithoutParameters: function () {
      var app = new FoxxController(fakeContext),
        routingInfo = app.routingInfo;

      assertEqual(routingInfo.routes.length, 0);
      assertEqual(routingInfo.urlPrefix, "");
    },

    testCreationWithURLPrefix: function () {
      var app = new FoxxController(fakeContext, {urlPrefix: "/wiese"}),
        routingInfo = app.routingInfo;

      assertEqual(routingInfo.routes.length, 0);
      assertEqual(routingInfo.urlPrefix, "/wiese");
    },

    testAdditionOfBaseMiddlewareInRoutingInfo: function () {
      var app = new FoxxController(fakeContext),
        routingInfo = app.routingInfo,
        hopefully_base = routingInfo.middleware[0];

      assertEqual(routingInfo.middleware.length, 1);
      assertEqual(hopefully_base.url.match, "/*");
    }
  };
}

function SetRoutesFoxxControllerSpec () {
  var app;

  return {
    setUp: function () {
      app = new FoxxController(fakeContext);
    },

    testSettingRoutes: function () {
      var myFunc = function () {},
        routes = app.routingInfo.routes;

      app.get('/simple/route', myFunc);
      assertEqual(routes.length, 1);
      assertEqual(routes[0].url.match, '/simple/route');
      assertUndefined(routes[0].url.constraint);
    },

    testSetMethodToHead: function () {
      var myFunc = function () {},
        routes = app.routingInfo.routes;

      app.head('/simple/route', myFunc);
      assertEqual(routes[0].docs.httpMethod, 'HEAD');
      assertEqual(routes[0].url.methods, ["head"]);
    },

    testSetMethodToGet: function () {
      var myFunc = function () {},
        routes = app.routingInfo.routes;

      app.get('/simple/route', myFunc);
      assertEqual(routes[0].docs.httpMethod, 'GET');
      assertEqual(routes[0].url.methods, ["get"]);
    },

    testSetMethodToPost: function () {
      var myFunc = function () {},
        routes = app.routingInfo.routes;

      app.post('/simple/route', myFunc);
      assertEqual(routes[0].docs.httpMethod, 'POST');
      assertEqual(routes[0].url.methods, ["post"]);
    },

    testSetMethodToPut: function () {
      var myFunc = function () {},
        routes = app.routingInfo.routes;

      app.put('/simple/route', myFunc);
      assertEqual(routes[0].docs.httpMethod, 'PUT');
      assertEqual(routes[0].url.methods, ["put"]);
    },

    testSetMethodToPatch: function () {
      var myFunc = function () {},
        routes = app.routingInfo.routes;

      app.patch('/simple/route', myFunc);
      assertEqual(routes[0].docs.httpMethod, 'PATCH');
      assertEqual(routes[0].url.methods, ["patch"]);
    },

    testSetMethodToDelete: function () {
      var myFunc = function () {},
        routes = app.routingInfo.routes;

      app['delete']('/simple/route', myFunc);
      assertEqual(routes[0].docs.httpMethod, 'DELETE');
      assertEqual(routes[0].url.methods, ["delete"]);
    },

    testSetMethodToDeleteViaAlias: function () {
      var myFunc = function () {},
        routes = app.routingInfo.routes;

      app.del('/simple/route', myFunc);
      assertEqual(routes[0].docs.httpMethod, 'DELETE');
      assertEqual(routes[0].url.methods, ["delete"]);
    },

    testRefuseRoutesWithRoutesThatAreNumbers: function () {
      var myFunc = function () {},
        routes = app.routingInfo.routes,
        error;

      try {
        app.get(2, myFunc);
      } catch(e) {
        error = e;
      }
      assertEqual(error, new Error("URL has to be a String"));
      assertEqual(routes.length, 0);
    },

    testRefuseRoutesWithRoutesThatAreRegularExpressions: function () {
      var myFunc = function () {},
        routes = app.routingInfo.routes,
        error;

      try {
        app.get(/[a-z]*/, myFunc);
      } catch(e) {
        error = e;
      }
      assertEqual(error, new Error("URL has to be a String"));
      assertEqual(routes.length, 0);
    },

    testRefuseRoutesWithRoutesThatAreArrays: function () {
      var myFunc = function () {},
        routes = app.routingInfo.routes,
        error;

      try {
        app.get(["/a", "/b"], myFunc);
      } catch(e) {
        error = e;
      }
      assertEqual(error, new Error("URL has to be a String"));
      assertEqual(routes.length, 0);
    },

    testAddALoginRoute: function () {
      var myFunc = function () {},
        routes = app.routingInfo.routes;

      app.activateAuthentication({
        type: "cookie",
        cookieLifetime: 360000,
        cookieName: "my_cookie",
        sessionLifetime: 400
      });
      app.login('/simple/route', myFunc);
      assertEqual(routes[0].docs.httpMethod, 'POST');
      assertEqual(routes[0].url.methods, ["post"]);
    },

    testRefuseLoginWhenAuthIsNotSetUp: function () {
      var myFunc = function () {},
        error;

      try {
        app.login('/simple/route', myFunc);
      } catch(e) {
        error = e;
      }

      assertEqual(error, new Error("Setup authentication first"));
    },

    testAddALogoutRoute: function () {
      var myFunc = function () {},
        routes = app.routingInfo.routes;

      app.activateAuthentication({
        type: "cookie",
        cookieLifetime: 360000,
        cookieName: "my_cookie",
        sessionLifetime: 400
      });
      app.logout('/simple/route', myFunc);
      assertEqual(routes[0].docs.httpMethod, 'POST');
      assertEqual(routes[0].url.methods, ["post"]);
    },

    testRefuseLogoutWhenAuthIsNotSetUp: function () {
      var myFunc = function () {},
        error;

      try {
        app.logout('/simple/route', myFunc);
      } catch(e) {
        error = e;
      }

      assertEqual(error, new Error("Setup authentication first"));
    },
  };
}

function DocumentationAndConstraintsSpec () {
  var app, routes, models;

  return {
    setUp: function () {
      app = new FoxxController(fakeContext);
      routes = app.routingInfo.routes;
      models = app.models;
    },

    testDefinePathParam: function () {
      app.get('/foxx/:id', function () {
        //nothing
      }).pathParam("id", {
        description: "Id of the Foxx",
        type: "int"
      });

      assertEqual(routes.length, 1);
      assertEqual(routes[0].url.constraint.id, "/[0-9]+/");
      assertEqual(routes[0].docs.parameters[0].paramType, "path");
      assertEqual(routes[0].docs.parameters[0].name, "id");
      assertEqual(routes[0].docs.parameters[0].description, "Id of the Foxx");
      assertEqual(routes[0].docs.parameters[0].dataType, "int");
    },

    testDefinePathCaseParam: function () {
      app.get('/foxx/:idParam', function () {
        //nothing
      }).pathParam("idParam", {
        description: "Id of the Foxx",
        type: "int"
      });

      assertEqual(routes.length, 1);
      assertEqual(routes[0].url.constraint.idParam, "/[0-9]+/");
      assertEqual(routes[0].docs.parameters[0].paramType, "path");
      assertEqual(routes[0].docs.parameters[0].name, "idParam");
      assertEqual(routes[0].docs.parameters[0].description, "Id of the Foxx");
      assertEqual(routes[0].docs.parameters[0].dataType, "int");
    },

    testDefineMultiplePathParams: function () {
      app.get('/:foxx/:id', function () {
        //nothing
      }).pathParam("foxx", {
        description: "Kind of Foxx",
        type: "string"
      }).pathParam("id", {
        description: "Id of the Foxx",
        type: "int"
      });

      assertEqual(routes.length, 1);

      assertEqual(routes[0].url.constraint.foxx, "/.+/");
      assertEqual(routes[0].docs.parameters[0].paramType, "path");
      assertEqual(routes[0].docs.parameters[0].name, "foxx");
      assertEqual(routes[0].docs.parameters[0].description, "Kind of Foxx");
      assertEqual(routes[0].docs.parameters[0].dataType, "string");

      assertEqual(routes[0].url.constraint.id, "/[0-9]+/");
      assertEqual(routes[0].docs.parameters[1].paramType, "path");
      assertEqual(routes[0].docs.parameters[1].name, "id");
      assertEqual(routes[0].docs.parameters[1].description, "Id of the Foxx");
      assertEqual(routes[0].docs.parameters[1].dataType, "int");
    },

    testDefineMultiplePathCaseParams: function () {
      app.get('/:foxxParam/:idParam', function () {
        //nothing
      }).pathParam("foxxParam", {
        description: "Kind of Foxx",
        type: "string"
      }).pathParam("idParam", {
        description: "Id of the Foxx",
        type: "int"
      });

      assertEqual(routes.length, 1);

      assertEqual(routes[0].url.constraint.foxxParam, "/.+/");
      assertEqual(routes[0].docs.parameters[0].paramType, "path");
      assertEqual(routes[0].docs.parameters[0].name, "foxxParam");
      assertEqual(routes[0].docs.parameters[0].description, "Kind of Foxx");
      assertEqual(routes[0].docs.parameters[0].dataType, "string");

      assertEqual(routes[0].url.constraint.idParam, "/[0-9]+/");
      assertEqual(routes[0].docs.parameters[1].paramType, "path");
      assertEqual(routes[0].docs.parameters[1].name, "idParam");
      assertEqual(routes[0].docs.parameters[1].description, "Id of the Foxx");
      assertEqual(routes[0].docs.parameters[1].dataType, "int");
    },

    testDefineQueryParam: function () {
      app.get('/foxx', function () {
        //nothing
      }).queryParam("a", {
        description: "The value of an a",
        type: "int",
        required: false,
        allowMultiple: true
      });

      assertEqual(routes.length, 1);
      assertEqual(routes[0].docs.parameters[0].paramType, "query");
      assertEqual(routes[0].docs.parameters[0].name, "a");
      assertEqual(routes[0].docs.parameters[0].description, "The value of an a");
      assertEqual(routes[0].docs.parameters[0].dataType, "int");
      assertEqual(routes[0].docs.parameters[0].required, false);
      assertEqual(routes[0].docs.parameters[0].allowMultiple, true);
    },

    testAddBodyParam: function () {
      var determinedName,
        determinedType,
        paramName = stub(),
        description = stub(),
        ModelPrototype = stub(),
        jsonSchema = { id: 'a', required: [], properties: {} };

      allow(ModelPrototype)
        .toReceive("toJSONSchema")
        .andReturn(jsonSchema);

      app.get('/foxx', function () {
        //nothing
      }).bodyParam(paramName, description, ModelPrototype);

      assertEqual(routes.length, 1);
      assertEqual(routes[0].docs.parameters[0].name, paramName);
      assertEqual(routes[0].docs.parameters[0].paramType, "body");
      assertEqual(routes[0].docs.parameters[0].description, description);
      assertEqual(routes[0].docs.parameters[0].dataType, jsonSchema.id);
    },

    testDefineBodyParamAddsJSONSchemaToModels: function () {
      var determinedName,
        determinedType,
        paramName = stub(),
        description = stub(),
        ModelPrototype = stub(),
        jsonSchema = { id: 'a', required: [], properties: {} };

      allow(ModelPrototype)
        .toReceive("toJSONSchema")
        .andReturn(jsonSchema);

      app.get('/foxx', function () {
        //nothing
      }).bodyParam(paramName, description, ModelPrototype);

      assertEqual(app.models[jsonSchema.id], jsonSchema);
    },

    testSetParamForBodyParam: function () {
      var req = { parameters: {} },
        res = {},
        paramName = stub(),
        description = stub(),
        requestBody = stub(),
        ModelPrototype = stub(),
        jsonSchemaId = stub(),
        called = false;

      allow(req)
        .toReceive("body")
        .andReturn(requestBody);

      ModelPrototype = mockConstructor(requestBody);
      ModelPrototype.toJSONSchema = function () { return { id: jsonSchemaId }; };

      app.get('/foxx', function (providedReq) {
        called = (providedReq.parameters[paramName] instanceof ModelPrototype);
      }).bodyParam(paramName, description, ModelPrototype);

      routes[0].action.callback(req, res);

      assertTrue(called);
      ModelPrototype.assertIsSatisfied();
    },

    testDocumentationForErrorResponse: function () {
      var CustomErrorClass = function () {};

      app.get('/foxx', function () {
        //nothing
      }).errorResponse(CustomErrorClass, 400, "I don't understand a word you're saying");

      assertEqual(routes.length, 1);
      assertEqual(routes[0].docs.errorResponses.length, 1);
      assertEqual(routes[0].docs.errorResponses[0].code, 400);
      assertEqual(routes[0].docs.errorResponses[0].reason, "I don't understand a word you're saying");
    },

    testCatchesDefinedError: function () {
      var CustomErrorClass = function () {},
        req = {},
        res,
        code = 400,
        reason = "This error was really... something!",
        statusWasCalled = false,
        jsonWasCalled = false,
        passedRequestAndResponse = false;

      res = {
        status: function (givenCode) {
          statusWasCalled = (givenCode === code);
        },
        json: function (givenBody) {
          jsonWasCalled = (givenBody.error === reason);
        }
      };

      app.get('/foxx', function (providedReq, providedRes) {
        if (providedReq === req && providedRes === res) {
          passedRequestAndResponse = true;
        }
        throw new CustomErrorClass();
      }).errorResponse(CustomErrorClass, code, reason);

      routes[0].action.callback(req, res);

      assertTrue(statusWasCalled);
      assertTrue(jsonWasCalled);
      assertTrue(passedRequestAndResponse);
    },

    testCatchesDefinedErrorWithCustomFunction: function () {
      var jsonWasCalled = false,
        req = {},
        res,
        code = 400,
        reason = "This error was really... something!",
        CustomErrorClass = function () {};

      res = {
        status: function () {},
        json: function (givenBody) {
          jsonWasCalled = givenBody.success;
        }
      };

      app.get('/foxx', function (providedReq, providedRes) {
        throw new CustomErrorClass();
      }).errorResponse(CustomErrorClass, code, reason, function (e) {
        if (e instanceof CustomErrorClass) {
          return { success: "true" };
        }
      });

      routes[0].action.callback(req, res);

      assertTrue(jsonWasCalled);
    },

    testControllerWideErrorResponse: function () {
      var CustomErrorClass = function () {};

      app.allRoutes.errorResponse(CustomErrorClass, 400, "I don't understand a word you're saying");

      app.get('/foxx', function () {
        //nothing
      });

      assertEqual(routes.length, 1);
      assertEqual(routes[0].docs.errorResponses.length, 1);
      assertEqual(routes[0].docs.errorResponses[0].code, 400);
      assertEqual(routes[0].docs.errorResponses[0].reason, "I don't understand a word you're saying");
    }
  };
}

function AddMiddlewareFoxxControllerSpec () {
  var app;

  return {
    setUp: function () {
      app = new FoxxController(fakeContext);
    },

    testAddABeforeMiddlewareForAllRoutes: function () {
      var myFunc = function (req, res) { a = (req > res); },
        middleware = app.routingInfo.middleware,
        callback;

      app.before(myFunc);

      assertEqual(middleware.length, 2);
      assertEqual(middleware[1].url.match, '/*');
      callback = String(middleware[1].action.callback);
      assertTrue(callback.indexOf((String(myFunc) + "(req, res)") > 0));
      assertTrue(callback.indexOf("next()" > 0));
      assertTrue(callback.indexOf(String(myFunc)) < callback.indexOf("next()"));
    },

    testAddABeforeMiddlewareForCertainRoutes: function () {
      var myFunc = function (req, res) { a = (req > res); },
        middleware = app.routingInfo.middleware,
        callback;

      app.before('/fancy/path', myFunc);

      assertEqual(middleware.length, 2);
      assertEqual(middleware[1].url.match, '/fancy/path');
      callback = String(middleware[1].action.callback);
      assertTrue(callback.indexOf((String(myFunc) + "(req, res)") > 0));
      assertTrue(callback.indexOf("next()" > 0));
      assertTrue(callback.indexOf(String(myFunc)) < callback.indexOf("next()"));
    },

    testAddAnAfterMiddlewareForAllRoutes: function () {
      var myFunc = function (req, res) { a = (req > res); },
        middleware = app.routingInfo.middleware,
        callback;

      app.after(myFunc);

      assertEqual(middleware.length, 2);
      assertEqual(middleware[1].url.match, '/*');
      callback = String(middleware[1].action.callback);
      assertTrue(callback.indexOf((String(myFunc) + "(req, res)") > 0));
      assertTrue(callback.indexOf("next()" > 0));
      assertTrue(callback.indexOf("func(req") > callback.indexOf("next()"));
    },

    testAddAnAfterMiddlewareForCertainRoutes: function () {
      var myFunc = function (req, res) { a = (req > res); },
        middleware = app.routingInfo.middleware,
        callback;

      app.after('/fancy/path', myFunc);

      assertEqual(middleware.length, 2);
      assertEqual(middleware[1].url.match, '/fancy/path');
      callback = String(middleware[1].action.callback);
      assertTrue(callback.indexOf((String(myFunc) + "(req, res)") > 0));
      assertTrue(callback.indexOf("next()" > 0));
      assertTrue(callback.indexOf("func(req") > callback.indexOf("next()"));
    }
  };
}

function CommentDrivenDocumentationSpec () {
  var app, routingInfo, noop;

  return {
    setUp: function () {
      app = new FoxxController(fakeContext);
      routingInfo = app.routingInfo;
      noop = function () {};
    },

    testSettingTheSummary: function () {
      fakeContext.comments = [
        "Get all the foxes",
        "A function to get all foxes from the database",
        "in a good way."
      ];

      app.get('/simple/route', noop);

      assertEqual(routingInfo.routes[0].docs.summary, "Get all the foxes");
    },

    testSettingTheNotes: function () {
      fakeContext.comments = [
        "Get all the foxes",
        "A function to get all foxes from the database",
        "in a good way."
      ];

      app.get('/simple/route', noop);

      assertEqual(routingInfo.routes[0].docs.notes, "A function to get all foxes from the database\nin a good way.");
    },

    testSettingTheSummaryWithAnEmptyFirstLine: function () {
      fakeContext.comments = [
        "",
        "Get all the foxes"
      ];

      app.get('/simple/route', noop);

      assertEqual(routingInfo.routes[0].docs.summary, "Get all the foxes");
    },

    testCleanUpCommentsAfterwards: function () {
      var clearCommentsWasCalled = false;
      fakeContext.clearComments = function () { clearCommentsWasCalled = true; };
      fakeContext.comments = [
        "Get all the foxes",
        "A function to get all foxes from the database",
        "in a good way."
      ];

      app.get('/simple/route', noop);
      assertTrue(clearCommentsWasCalled);
    },

    testSetBothToEmptyStringsIfTheJSDocWasEmpty: function () {
      fakeContext.comments = [
        "",
        "",
        ""
      ];

      app.get('/simple/route', noop);
      assertEqual(routingInfo.routes[0].docs.summary, "");
      assertEqual(routingInfo.routes[0].docs.notes, "");
    }
  };
}

function HelperFunctionSpec () {
  var app;

  return {
    setUp: function () {
      fakeContext.collectionPrefix = "fancy";
      app = new FoxxController(fakeContext);
    },

    testGetACollection: function () {
      db._create("fancy_pants");

      assertEqual(app.collection("pants"), db._collection("fancy_pants"));
    },

    testGetACollectionThatDoesNotExist: function () {
      var err;
      db._drop("fancy_pants");

      try {
        app.collection("pants");
      } catch(e) {
        err = e;
      }

      assertEqual(err.message, "collection with name 'fancy_pants' does not exist.");
    }
  };
}

function SetupAuthorization () {
  var app;

  return {
    testWorksWithAllParameters: function () {
      var err;

      app = new FoxxController(fakeContext);

      try {
        app.activateAuthentication({
          type: "cookie",
          cookieLifetime: 360000,
          cookieName: "mycookie",
          sessionLifetime: 600
        });
      } catch (e) {
        err = e;
      }

      assertUndefined(err);
    },

    testRefusesUnknownAuthTypes: function () {
      var err;

      app = new FoxxController(fakeContext);

      try {
        app.activateAuthentication({
          type: "brainwave",
          cookieLifetime: 360000,
          cookieName: "mycookie",
          sessionLifetime: 600
        });
      } catch (e) {
        err = e;
      }

      assertEqual(err.message, "Currently only the following auth types are supported: cookie");
    },

    testRefusesMissingCookieLifetime: function () {
      var err;

      app = new FoxxController(fakeContext);

      try {
        app.activateAuthentication({
          type: "cookie",
          cookieName: "mycookie",
          sessionLifetime: 600
        });
      } catch (e) {
        err = e;
      }

      assertEqual(err.message, "Please provide the cookieLifetime");
    },

    testRefusesMissingCookieName: function () {
      var err;

      app = new FoxxController(fakeContext);

      try {
        app.activateAuthentication({
          type: "cookie",
          cookieLifetime: 360000,
          sessionLifetime: 600
        });
      } catch (e) {
        err = e;
      }

      assertEqual(err.message, "Please provide the cookieName");
    },

    testRefusesMissingSessionLifetime: function () {
      var err;

      app = new FoxxController(fakeContext);

      try {
        app.activateAuthentication({
          type: "cookie",
          cookieName: "mycookie",
          cookieLifetime: 360000
        });
      } catch (e) {
        err = e;
      }

      assertEqual(err.message, "Please provide the sessionLifetime");
    }
  };
}

jsunity.run(CreateFoxxControllerSpec);
jsunity.run(SetRoutesFoxxControllerSpec);
jsunity.run(DocumentationAndConstraintsSpec);
jsunity.run(AddMiddlewareFoxxControllerSpec);
jsunity.run(CommentDrivenDocumentationSpec);
jsunity.run(HelperFunctionSpec);
jsunity.run(SetupAuthorization);

return jsunity.done();
