$(document).ready(function () {
    stripTemplate                   = $.templates("#lightsource-strip-template");
    sliceTemplate                   = $.templates("#lightsource-slice-template");
    $.templates("lightsource-slice-template", "#lightsource-slice-template");

    $.jsonRPC.request("init", {success: remoteConfigReceived, error: remoteConfigReceptionError});

    $(".lightsource-test-config").click(function (e){
        console.log("onClick: .lightsource-test-config");
        console.log($('#lightsource-config-form').serializeObject());
        $.jsonRPC.request("testConfig",
            {
                params:[$('form#lightsource-config-form').serializeObject().strips],
                success: testConfigReceived,
                error: testConfigReceptionError
            }
        );
    });

});

$.jsonRPC.setup({
    endPoint: "/devel/lightsource/data/rpc.php",
    namespace: "lightsource"
});

function addStrip(e)
{
    var helper = {stripIndex: lastStripIndex};
    $("#lightsource-config-form").append(stripTemplate.render(createEmptyStrip(), helper));
    updateColorpickers();

    lastStripIndex++;
}

function addSlice(e)
{
    stripIndex = e.getAttribute("strip");
    $(sliceTemplate.render(createEmptySlice(), {stripIndex: stripIndex})).insertBefore($(e).parent().parent());
    updateColorpickers();
}

function removeSlice(e)
{
    $(e).parent().parent().remove();
}

function removeStrip(e)
{
    $(e).parent().parent().parent().parent().remove();
}

function createEmptySlice()
{
    return ({name: "new slice", start: 0, end: 8, htmlColor: "#00ff00"});
}

function createEmptyStrip()
{
    strip = {
        name: "new strip",
        pin: 0,
        length: 8,
        htmlColor: "#ff0000",
        type: 0
    };

    return strip;
}

function renderStripsTable(strips)
{
    for (n=0; n<strips.length; n++)
    {
        var helper = {stripIndex: n};
        $("#lightsource-config-form").append(stripTemplate.render(strips[n], helper));
    }

    lastStripIndex = n;
}



function remoteConfigReceived(remoteConfig)
{
    console.log("remoteConfigReceived:");
    console.log(remoteConfig.result);

    if (remoteConfig.result.strips.length > 0)
    {
        renderStripsTable(remoteConfig.result.strips);
    }
    updateColorpickers();
}

function updateColorpickers()
{
    $(function()
    {
        $(".colorpicker-selector").colorpicker(
            {
                format: 'hex',
                align: 'left',
                customClass: 'colorpicker-2x',
                sliders: {
                    saturation: {
                        maxLeft: 200,
                        maxTop: 200
                    },
                    hue: {
                        maxTop: 200
                    },
                    alpha: {
                        maxTop: 200
                    }
                }
            }).on('changeColor',function(e)
            {
                $(e.target.previousElementSibling).css("background-color", e.value);
            }
        );
    });
}

function remoteConfigReceptionError(o)
{
    console.log("Error receiving initial config from remote server");
}

function testConfigReceived(o)
{
    console.log("Config test success");
}

function testConfigReceptionError(o)
{
    console.log("Config test failed");
}