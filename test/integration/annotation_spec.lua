---
--- Created by rakesh.
--- DateTime: 06/06/2020 07:31
---

local log = require "test.integration.log"

local date = require "date"
local inspect = require "inspect"
local requests = require "requests"

describe("Grafana data source annotation endpoint test", function()
  local baseUrl = "http://localhost:8021"

  local function is_array(tbl) return type(tbl) == 'table' and (#tbl > 0 or next(tbl) == nil) end
  local function tablelength(T)
    local count = 0
    for _ in pairs(T) do count = count + 1 end
    return count
  end

  it("Simple annotation query test", function()
    local from = date(true)
    assert.has_no.errors(function() from:adddays(-30) end)
    local to = date(true)

    local data = {
      range = {
        from = from:fmt("%Y-%m-%dT%H:%M:%S.000Z"),
        to = to:fmt("%Y-%m-%dT%H:%M:%S.000Z"),
        raw = {
          from = "now-30d",
          to = "now"
        }
      },
      annotation = {
        name = "location",
        datasource = "SimpleJson",
        enable = true,
        iconColor = "rgba(255, 96, 96, 1)",
        query = "request.location"
      },
      rangeRaw = {
        from = "now-30d",
        to = "now"
      }
    }

    local headers = {["content-type"] = "application/json; charset=utf-8"}
    local response = requests.post{baseUrl.."/annotations", headers = headers, data = data, timeout = 2}
    assert.are.equal(200, response.status_code)
    assert.has_no.errors(function() response.json() end)
    local jsonBody = response.json()
    log.info(inspect(jsonBody))
  end)
end)
