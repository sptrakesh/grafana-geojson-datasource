---
--- Created by rakesh.
--- DateTime: 05/06/2020 21:49
---

local log = require "test.integration.log"

local date = require "date"
local inspect = require "inspect"
local requests = require "requests"

describe("Grafana data source query endpoint test", function()
  local baseUrl = "http://localhost:8020"

  local function is_array(tbl) return type(tbl) == 'table' and (#tbl > 0 or next(tbl) == nil) end
  local function tablelength(T)
    local count = 0
    for _ in pairs(T) do count = count + 1 end
    return count
  end

  it("Sample query with low limit", function()
    local from = date(true)
    assert.has_no.errors(function() from:adddays(-30) end)
    local to = date(true)

    local data = {
      app = "dashboard",
      requestId = "Q107",
      timezone = "browser",
      panelId = 4,
      dashboardId = 1,
      range = {
        from = from:fmt("%Y-%m-%dT%H:%M:%S.000Z"),
        to = to:fmt("%Y-%m-%dT%H:%M:%S.000Z"),
        raw = {
          from = "now-30d",
          to = "now"
        }
      },
      timeInfo = "",
      interval = "5m",
      intervalMs = 300000,
      targets = {{
        target = "request.location",
        refId = "A",
        type = timeserie
      }},
      maxDataPoints = 1,
      scopedVars = {
        __interval = {
          text = "5m",
          value = "5m"
        },
        __interval_ms = {
          text = "300000",
          value = 300000
        }
      },
      startTime = 1591411744058,
      rangeRaw = {
        from = "now-30d",
        to = "now"
      }
    }
    local headers = {["content-type"] = "application/json; charset=utf-8"}
    local response = requests.post{baseUrl.."/query", headers = headers, data = data, timeout = 2}
    assert.are.equal(200, response.status_code)
    assert.has_no.errors(function() response.json() end)
    local jsonBody = response.json()
    log.info(inspect(jsonBody))
    assert.is.truthy(is_array(jsonBody))
    assert.is.equal(tablelength(jsonBody), 1)
    assert.is.truthy(jsonBody[1])
    assert.is.truthy(is_array(jsonBody[1]["columns"]))
    assert.is.equal(tablelength(jsonBody[1]["columns"]), 1)
    assert.is.truthy(is_array(jsonBody[1]["rows"]))
    assert.is.equal(tablelength(jsonBody[1]["rows"]), 1)
  end)

  it("Sample query with two targets", function()
    local from = date(true)
    assert.has_no.errors(function() from:adddays(-30) end)
    local to = date(true)

    local data = {
      app = "dashboard",
      requestId = "Q107",
      timezone = "browser",
      panelId = 4,
      dashboardId = 1,
      range = {
        from = from:fmt("%Y-%m-%dT%H:%M:%S.000Z"),
        to = to:fmt("%Y-%m-%dT%H:%M:%S.000Z"),
        raw = {
          from = "now-30d",
          to = "now"
        }
      },
      timeInfo = "",
      interval = "5m",
      intervalMs = 300000,
      targets = {{
                   target = "request.location",
                   refId = "A",
                   type = timeserie
                 },
                 {
                   target = "request.location",
                   refId = "B",
                   type = timeserie
                 }},
      maxDataPoints = 1,
      scopedVars = {
        __interval = {
          text = "5m",
          value = "5m"
        },
        __interval_ms = {
          text = "300000",
          value = 300000
        }
      },
      startTime = 1591411744058,
      rangeRaw = {
        from = "now-30d",
        to = "now"
      }
    }
    local headers = {["content-type"] = "application/json; charset=utf-8"}
    local response = requests.post{baseUrl.."/query", headers = headers, data = data, timeout = 2}
    assert.are.equal(200, response.status_code)
    assert.has_no.errors(function() response.json() end)
    local jsonBody = response.json()
    assert.is.truthy(is_array(jsonBody))
    assert.is.equal(tablelength(jsonBody), 2)
    assert.is.truthy(jsonBody[1])
    assert.is.truthy(jsonBody[2])
    assert.is.truthy(is_array(jsonBody[1]["columns"]))
    assert.is.truthy(is_array(jsonBody[2]["columns"]))
    assert.is.equal(tablelength(jsonBody[1]["columns"]), 1)
    assert.is.equal(tablelength(jsonBody[2]["columns"]), 1)
    assert.is.truthy(is_array(jsonBody[1]["rows"]))
    assert.is.truthy(is_array(jsonBody[2]["rows"]))
    assert.is.equal(tablelength(jsonBody[1]["rows"]), 1)
    assert.is.equal(tablelength(jsonBody[2]["rows"]), 1)
  end)
end)
