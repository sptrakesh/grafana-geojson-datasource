---
--- Created by rakesh.
--- DateTime: 05/06/2020 21:16
---

local log = require "test.integration.log"
local requests = require "requests"

describe("Grafana data source search endpoint test", function()
  local baseUrl = "http://localhost:8020"
  local function is_array(tbl) return type(tbl) == 'table' and (#tbl > 0 or next(tbl) == nil) end
  local series = ""

  it("Find all event series names", function()
    local data = {target = ""}
    local headers = {["content-type"] = "application/json; charset=utf-8"}
    local response = requests.post{baseUrl.."/search", headers = headers, data = data, timeout = 2}
    assert.are.equal(200, response.status_code)
    assert.has_no.errors(function() response.json() end)
    local jsonBody = response.json()
    assert.is.truthy(is_array(jsonBody))
    assert.is.truthy(#jsonBody > 0)
    log.info(jsonBody[1])
    series = jsonBody[1]
  end)

  it("Find all event series names with initial !", function()
    local data = {target = "!"}
    local headers = {["content-type"] = "application/json; charset=utf-8"}
    local response = requests.post{baseUrl.."/search", headers = headers, data = data, timeout = 2}
    assert.are.equal(200, response.status_code)
    assert.has_no.errors(function() response.json() end)
    local jsonBody = response.json()
    assert.is.truthy(is_array(jsonBody))
    assert.is.truthy(#jsonBody > 0)
    log.info(jsonBody[1])
  end)

  it("Find event series matching prefix", function()
    local data = {target = series:sub(0, 2)}
    local headers = {["content-type"] = "application/json; charset=utf-8"}
    local response = requests.post{baseUrl.."/search", headers = headers, data = data, timeout = 2}
    assert.are.equal(200, response.status_code)
    assert.has_no.errors(function() response.json() end)
    local jsonBody = response.json()
    assert.is.truthy(is_array(jsonBody))
    assert.is.truthy(#jsonBody > 0)
    log.info(jsonBody[1])
  end)

  it("Find event series matching prefix with initial !", function()
    local data = {target = "!"..series:sub(0, 2)}
    local headers = {["content-type"] = "application/json; charset=utf-8"}
    local response = requests.post{baseUrl.."/search", headers = headers, data = data, timeout = 2}
    assert.are.equal(200, response.status_code)
    assert.has_no.errors(function() response.json() end)
    local jsonBody = response.json()
    assert.is.truthy(is_array(jsonBody))
    assert.is.truthy(#jsonBody > 0)
    log.info(jsonBody[1])
  end)
end)
