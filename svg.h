#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

namespace svg {

    // --- Object Properties ----------------------------------------------------------
    // ------ color -------------------------------------------------------------------

    struct Rgb {
        Rgb() = default;
        Rgb(unsigned int r, unsigned int g, unsigned int b)
            : red(r), green(g), blue(b)
        {
        }//*/
        uint8_t red = 0;
        uint8_t green = 0;
        uint8_t blue = 0;
    };

    struct Rgba {
        Rgba() = default;
        Rgba(unsigned int r, unsigned int g, unsigned int b, double o)
            : red(r), green(g), blue(b), opacity(o)
        {
        }//*/
        uint8_t red = 0;
        uint8_t green = 0;
        uint8_t blue = 0;
        double opacity = 1.;
    };

    using Color = std::variant<std::monostate, std::string, Rgb, Rgba>;
    inline const Color NoneColor{};

    struct OstreamColor {
        std::ostream& out;

        void operator()(std::monostate) const {
            out << "none";
        }
        void operator()(const std::string s) const {
            out << s;
        }
        void operator()(Rgb rgb) const {
            out << "rgb("
                << std::to_string(rgb.red) << ","
                << std::to_string(rgb.green) << ","
                << std::to_string(rgb.blue) << ")";
        }
        void operator()(Rgba rgba) const {
            out << "rgba("
                << std::to_string(rgba.red) << ","
                << std::to_string(rgba.green) << ","
                << std::to_string(rgba.blue) << ","
                << rgba.opacity << ")";
        }
    };

    std::ostream& operator<<(std::ostream& out, Color color);

    // ------ other properties --------------------------------------------------------
    enum class StrokeLineCap {
        BUTT,
        ROUND,
        SQUARE,
    };

    enum class StrokeLineJoin {
        ARCS,
        BEVEL,
        MITER,
        MITER_CLIP,
        ROUND,
    };

    std::ostream& operator<<(std::ostream& out, const StrokeLineCap line);
    std::ostream& operator<<(std::ostream& out, const StrokeLineJoin line);

    // --- Point ----------------------------------------------------------------------
    struct Point {
        Point() = default;
        Point(double x, double y)
            : x(x)
            , y(y) {
        }
        double x = 0;
        double y = 0;
    };

    // --- RenderContext --------------------------------------------------------------
    // ��������������� ���������, �������� �������� ��� ������ SVG-��������� � ���������.
    // ������ ������ �� ����� ������, ������� �������� � ��� ������� ��� ������ �������� 
    struct RenderContext {
        RenderContext(std::ostream& out)
            : out(out) {
        }

        RenderContext(std::ostream& out, int indent_step, int indent = 0)
            : out(out)
            , indent_step(indent_step)
            , indent(indent) {
        }

        RenderContext Indented() const {
            return { out, indent_step, indent + indent_step };
        }

        void RenderIndent() const {
            for (int i = 0; i < indent; ++i) {
                out.put(' ');
            }
        }

        std::ostream& out;
        int indent_step = 0;
        int indent = 0;
    };

    // --- Object ---------------------------------------------------------------------
    // ����������� ������� ����� Object ������ ��� ���������������� ��������
    // ���������� ����� SVG-���������
    // ��������� ������� "��������� �����" ��� ������ ����������� ����
    class Object {
    public:
        void Render(const RenderContext& context) const;

        virtual ~Object() = default;

    private:
        virtual void RenderObject(const RenderContext& context) const = 0;
    };

    // --- PathProps ------------------------------------------------------------------
    // ������� ����� ��� ������ � ������ (� ������� �����������) �������� Circle, Polyline, Text,
    // ���������, ����� ���������� ���������, ����� ������������ method chaining (a.f1().f2()...)
    template <typename Owner>
    class PathProps {
    public:
        Owner& SetFillColor(Color color);
        Owner& SetStrokeColor(Color color);
        Owner& SetStrokeWidth(double width);
        Owner& SetStrokeLineCap(StrokeLineCap line_cap);
        Owner& SetStrokeLineJoin(StrokeLineJoin line_join);

    protected:
        ~PathProps() = default;

        void RenderAttrs(std::ostream& out) const;

    private:
        Owner& AsOwner() {
            // static_cast ��������� ����������� *this � Owner&,
            // ���� ����� Owner � ��������� PathProps
            return static_cast<Owner&>(*this);
        }

        std::optional<Color> fill_color_;
        std::optional<Color> stroke_color_;
        std::optional<double> width_;
        std::optional<StrokeLineCap> line_cap_;
        std::optional<StrokeLineJoin> line_join_;
    };

    // --- Circle ---------------------------------------------------------------------
    // ����� Circle ���������� ������� <circle> ��� ����������� �����
    // https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle
    class Circle final : public Object, public PathProps<Circle> {
    public:
        Circle& SetCenter(Point center);
        Circle& SetRadius(double radius);

    private:
        void RenderObject(const RenderContext& context) const override;

        Point center_;
        double radius_ = 1.0;
    };

    // --- Polyline -------------------------------------------------------------------
    // ����� Polyline ���������� ������� <polyline> ��� ����������� ������� �����
    // https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
    class Polyline final : public Object, public PathProps<Polyline> {
    public:
        // ��������� ��������� ������� � ������� �����
        Polyline& AddPoint(Point point);

    private:
        void RenderObject(const RenderContext& context) const override;

        std::vector<Point> points_;
    };

    // --- Text -----------------------------------------------------------------------
    // ����� Text ���������� ������� <text> ��� ����������� ������
    // https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
    class Text final : public Object, public PathProps<Text> {
    public:
        Text() = default;

        // ����� ���������� ������� ����� (�������� x � y)
        Text& SetPosition(Point pos);

        // ����� �������� ������������ ������� ����� (�������� dx, dy)
        Text& SetOffset(Point offset);

        // ����� ������� ������ (������� font-size)
        Text& SetFontSize(uint32_t size);

        // ����� �������� ������ (������� font-family)
        Text& SetFontFamily(std::string font_family);

        // ����� ������� ������ (������� font-weight)
        Text& SetFontWeight(std::string font_weight);

        // ����� ��������� ���������� ������� (������������ ������ ���� text)
        Text& SetData(std::string data);

    private:
        void RenderObject(const RenderContext& context) const override;

        Point pos_ = { 0.0, 0.0 };
        Point offset_ = { 0.0, 0.0 };
        uint32_t size_ = 1;
        std::string font_family_ = "";
        std::string font_weight_ = "";
        std::string data_ = "";
    };


    // --- Object Container ----------------------------------------------------------
    // ����������� �����-��������� ��� ����������� ��������� ��������
    class ObjectContainer {
    public:
        virtual ~ObjectContainer() = default;

        // ������� �� ����� ���� ������������, ������� ������������ �����
        template <typename Obj>
        void Add(Obj obj) {
            AddPtr(std::make_unique<Obj>(obj));
        }

        virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;
    };

    // --- Drawable -------------------------------------------------------------------
    class Drawable {
    public:
        virtual ~Drawable() = default;
        virtual void Draw(ObjectContainer& oc) const = 0;
    };


    // --- Document -------------------------------------------------------------------
    class Document : public ObjectContainer {
    public:

        // ����� Add ��������� � svg-�������� ����� ������-��������� svg::Object.
        // (�� ����� �����, �.� ���������� ObjectContainer)
        /*template <typename Obj>
        void Add(Obj obj);//*/

        // ��������� � svg-�������� ������-��������� svg::Object
        void AddPtr(std::unique_ptr<Object>&& obj) override;

        // ������� � ostream svg-������������� ���������
        void Render(std::ostream& out) const;

    private:
        std::vector<std::unique_ptr<Object>> objects_;
    };


    // --- PathProps ------------------------------------------------------------------
    // ----- methods ------------------------------------------------------------------
    template <typename Owner>
    Owner& PathProps<Owner>::SetFillColor(Color color) {
        fill_color_ = std::move(color);
        return AsOwner();
    }

    template <typename Owner>
    Owner& PathProps<Owner>::SetStrokeColor(Color color) {
        stroke_color_ = std::move(color);
        return AsOwner();
    }

    template <typename Owner>
    Owner& PathProps<Owner>::SetStrokeWidth(double width) {
        width_ = width;
        return AsOwner();
    }
    template <typename Owner>
    Owner& PathProps<Owner>::SetStrokeLineCap(StrokeLineCap line_cap) {
        line_cap_ = line_cap;
        return AsOwner();
    }
    template <typename Owner>
    Owner& PathProps<Owner>::SetStrokeLineJoin(StrokeLineJoin line_join) {
        line_join_ = line_join;
        return AsOwner();
    }

    template <typename Owner>
    void PathProps<Owner>::RenderAttrs(std::ostream& out) const {
        using namespace std::literals;


        if (fill_color_) {
            out << " fill=\""sv << *fill_color_ << "\""sv;
        }
        if (stroke_color_) {
            out << " stroke=\""sv << *stroke_color_ << "\""sv;
        }
        if (width_) {
            out << " stroke-width=\""sv << *width_ << "\""sv;
        }
        if (line_cap_) {
            out << " stroke-linecap=\""sv << *line_cap_ << "\""sv;
        }
        if (line_join_) {
            out << " stroke-linejoin=\""sv << *line_join_ << "\""sv;
        }
    }

}  // namespace svg
