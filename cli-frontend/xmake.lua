add_repositories("tapzcrew-repo https://github.com/tapzcrew/xmake-repo main")

-- add_requires("frozen", "unordered_dense")
-- add_requires("stormkit develop", {
--   config = {
--     image = false,
--     wsi = false,
--     log = false,
--     entities = false,
--     gpu = false,
--     examples = false,
--     tests = false,
--     shared = true,
--   }
-- })
--                  v     last commit before the "Big Rename" breaking commit
add_requires("ftxui 2f59e5f6f154d040353fad0b1c7c5bff141a6f97", { configs = { modules = true } })

target("via-vocis-cli", function()
  set_languages("c++26")
  
  -- add_packages("stormkit", { components = { "core" } })
  -- add_packages("frozen", "unordered_dense")
  add_packages("ftxui")
  
  add_files("src/*.cpp", "src/*.cppm")
  
  set_policy("build.c++.modules", true)
end)
